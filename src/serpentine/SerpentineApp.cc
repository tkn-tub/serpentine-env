//
// Copyright (C) 2020 Dominik S. Buse <buse@ccs-labs.org>
//
// Documentation for these modules is at http://veins.car2x.org/
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

#include "serpentine/SerpentineApp.h"

#include "veins/base/utils/FindModule.h"
#include "veins/base/modules/BaseMobility.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/messages/DemoSafetyMessage_m.h"
#include "veins/modules/utility/Consts80211p.h"

Define_Module(veins::serpentine::SerpentineApp);


namespace veins {
namespace serpentine {

void SerpentineApp::initialize(int stage)
{
    BaseApplLayer::initialize(stage);

    if (stage == 0) {
        // set up beaconing timer
        auto triggerBeacon = [this]() { this->beacon(); };
        auto timerSpec = TimerSpecification(triggerBeacon)
            .relativeStart(uniform(0, par("beaconInterval")))
            .interval(par("beaconInterval"));
        timerManager.create(timerSpec);

        // find mobility submodule
        auto mobilityModules = getSubmodulesOfType<TraCIMobility>(getParentModule());
        ASSERT(mobilityModules.size() == 1);
        mobility = mobilityModules.front();

        // determine leader/follower role
        isFollower = mobilityModules.front()->getExternalId() == "follower";
        EV_INFO << "Initialized vehicle '" << mobilityModules.front()->getExternalId() << "' as " << (isFollower ? "Follower" : "Leader") << "\n";
    }
}

void SerpentineApp::finish()
{
}

void SerpentineApp::handleSelfMsg(cMessage* msg)
{
    timerManager.handleMessage(msg);
}

void SerpentineApp::beacon()
{
    // just some demo content
    auto* dsm = new DemoSafetyMessage();

    dsm->setRecipientAddress(LAddress::L2BROADCAST());
    dsm->setBitLength(par("headerLength").intValue());
    dsm->setSenderPos(mobility->getPositionAt(simTime()));
    dsm->setSenderSpeed(mobility->getCurrentSpeed());
    dsm->setPsid(-1);
    dsm->setChannelNumber(static_cast<int>(Channel::cch));
    dsm->addBitLength(par("beaconLengthBits").intValue());
    dsm->setUserPriority(par("beaconUserPriority").intValue());

    sendDown(dsm);
}

void SerpentineApp::handleLowerMsg(cMessage* msg)
{
    EV_INFO << "Received beacon.\n";
    cancelAndDelete(msg);
}

} // namespace serpentine
} // namespace veins
