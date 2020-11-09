//
// Copyright (C) 2019-2020 Dominik S. Buse <buse@ccs-labs.org>, Max Schettler <schettler@ccs-labs.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "GymSplitter.h"

#include <cmath>
#include <cstdlib>
#include <array>
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"

Define_Module(GymSplitter);

GymSplitter::~GymSplitter() {
    if (gymCon) {
        veinsgym::proto::Request request;
        request.set_id(1);
		*(request.mutable_shutdown()) = {};
		auto response = gymCon->communicate(request);
    }
}

void GymSplitter::initialize()
{
    veins::Splitter::initialize();

    // find mobility submodule
    auto mobilityModules = veins::getSubmodulesOfType<TraCIMobility>(getParentModule());
    ASSERT(mobilityModules.size() == 1);
    mobility = mobilityModules.front();

    // determine leader/follower role
    isFollower = mobilityModules.front()->getExternalId() == "follower";
    EV_INFO << "Initialized vehicle '" << mobilityModules.front()->getExternalId() << "' as " << (isFollower ? "Follower" : "Leader") << "\n";

    // set up socket for follower vehicle
    if (isFollower) {
        gymCon = veins::FindModule<GymConnection*>::findGlobalModule();
        ASSERT(gymCon);

        // update headway if specified
        double desiredHeadway = par("desiredHeadway");
        if (desiredHeadway > 0) {
            // set via traci
            const auto manager = veins::TraCIScenarioManagerAccess().get();
            auto iface = manager->getCommandInterface();
            auto traci_vehicle = iface->vehicle(mobilityModules.front()->getExternalId());
            traci_vehicle.setTau(desiredHeadway);
        }

        success_reward = par("successReward");
        dsrc_cost = par("dsrcCost");
        vlc_cost = par("vlcCost");
        max_range = par("maxRange");
    }
}

GymSplitter::Interfaces GymSplitter::getAccessTechnology(cPacket *msg) {
    Interfaces result = {Interface::dsrc, Interface::vlc_head, Interface::vlc_tail};
    const auto manager = veins::TraCIScenarioManagerAccess().get();
    const auto managedModules = manager->getManagedHosts();
    if (gymCon && managedModules.find("leader") != managedModules.end()) {
        const auto leaderHost = managedModules.at("leader");
        const auto observation = computeObservation(veins::TraCIMobilityAccess().get(leaderHost));
        const auto reward = computeReward(veins::getSubmodulesOfType<GymSplitter>(leaderHost).front());
        const auto request = serializeObservation(observation, reward);

        auto response = gymCon->communicate(request);

        result = Interfaces(response.action().discrete().value());
    }
    EV_INFO << "Using the following access technologies: " << result.to_string() << " \n";
    lastChoice = result;
    lastSentId = msg->getTreeId();
    return result;
}

void GymSplitter::handleLowerMessage(cMessage* msg) {
    lastReceivedId = msg->getTreeId();
    return Splitter::handleLowerMessage(msg);
}

double GymSplitter::computeReward(const GymSplitter* leaderSplitter) const {
    const bool lastSuccessful = lastSentId > 0 && lastSentId == leaderSplitter->getLastReceivedId(); // test assumes: beacon interval >> E2E latency
    const auto transmission_reward = success_reward * lastSuccessful;
    const auto dsrc_penalty = dsrc_cost * lastChoice.test(Interface::dsrc);
    const auto vlc_head_penalty = vlc_cost * lastChoice.test(Interface::vlc_head);
    const auto vlc_tail_penalty = vlc_cost * lastChoice.test(Interface::vlc_tail);
    const auto transmission_penalty = dsrc_penalty + vlc_head_penalty + vlc_tail_penalty;
    return transmission_reward - transmission_penalty;
}

std::array<double, 4> GymSplitter::computeObservation(const TraCIMobility* leaderMobility) const {
    // So far uses an oracle for simplicity
    // Alternative: base on received beacons, included information age
    const auto leaderPosition = leaderMobility->getPositionAt(simTime());
    const auto leaderHeading = leaderMobility->getHeading().getRad();
    const auto followerPosition = mobility->getPositionAt(simTime());
    const auto followerHeading = mobility->getHeading().getRad();
    const auto txVec = (leaderPosition - followerPosition).rotatedYaw(followerHeading);
    const auto rxVec = (followerPosition - leaderPosition).rotatedYaw(leaderHeading);
    return {
        // scale and clip range to normalize observation values
        std::max(-1.0, std::min(static_cast<double>(txVec.x / max_range), 1.0)),
        std::max(-1.0, std::min(static_cast<double>(txVec.y / max_range), 1.0)),
        rxVec.x/rxVec.length(),
        rxVec.y/rxVec.length()
    };
}

veinsgym::proto::Request GymSplitter::serializeObservation(const std::array<double, 4> &observation, const double reward) const {
    veinsgym::proto::Request request;
    request.set_id(1);
    auto *values = request.mutable_step()->mutable_observation()->mutable_box()->mutable_values();
    *values = {observation.begin(), observation.end()};
    request.mutable_step()->mutable_reward()->mutable_box()->mutable_values()->Add();
    request.mutable_step()->mutable_reward()->mutable_box()->set_values(0, reward);
    return request;
}
