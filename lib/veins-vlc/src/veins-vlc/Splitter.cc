//
// Copyright (C) 2017 Agon Memedi <memedi@ccs-labs.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later
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

#include "veins-vlc/Splitter.h"
#include "veins-vlc/messages/VlcMessage_m.h"
#include "veins-vlc/utility/Utils.h"

using namespace veins;
using std::unique_ptr;

Define_Module(veins::Splitter);

void Splitter::initialize()
{
    // From upper layers --> lower layers
    fromApplication = findGate("applicationIn");
    toDsrcNic = findGate("nicOut");
    toVlcHead = findGate("nicVlcHeadOut");
    toVlcTail = findGate("nicVlcTailOut");

    // From lower layers --> upper layers
    toApplication = findGate("applicationOut");
    fromDsrcNic = findGate("nicIn");
    fromVlcHead = findGate("nicVlcHeadIn");
    fromVlcTail = findGate("nicVlcTailIn");

    // Module parameters
    draw = par("draw").boolValue();
    headHalfAngle = deg2rad(par("drawHeadHalfAngle").doubleValue());
    tailHalfAngle = deg2rad(par("drawTailHalfAngle").doubleValue());
    collectStatistics = par("collectStatistics").boolValue();
    debug = par("debug").boolValue();

    // Signals
    totalVlcDelaySignal = registerSignal("totalVlcDelay");
    headVlcDelaySignal = registerSignal("headVlcDelay");
    tailVlcDelaySignal = registerSignal("tailVlcDelay");

    // Other simulation modules
    mobility = FindModule<TraCIMobility*>::findSubModule(getParentModule());
    ASSERT(mobility);

    vlcPhys = getSubmodulesOfType<PhyLayerVlc>(getParentModule(), true);
    ASSERT(vlcPhys.size() > 0);

    annotationManager = AnnotationManagerAccess().getIfExists();
    ASSERT(annotationManager);
}

void Splitter::handleMessage(cMessage* msg)
{
    if (timerManager.handleMessage(msg)) return;

    if (msg->isSelfMessage()) {
        error("Self-message arrived!");
        delete msg;
        msg = NULL;
    }
    else {
        int arrivalGate = msg->getArrivalGateId();
        if (arrivalGate == fromApplication) {
            handleUpperMessage(msg);
        }
        // The arrival gate is not from the application, it'a from lower layers
        // TODO: add annotation drawings based on the destination technology
        else {
            EV_INFO << "Message from lower layers received!" << std::endl;
            handleLowerMessage(msg);
        }
    }
}

void Splitter::handleUpperMessage(cMessage* msg)
{
    unique_ptr<BaseFrame1609_4> wsm(check_and_cast<BaseFrame1609_4*>(msg));
    const auto accessTechnology = getAccessTechnology(wsm.get());

    if (accessTechnology.test(Splitter::Interface::dsrc)) {
        EV_INFO << "DSRC message received from upper layer!" << std::endl;
        send(wsm->dup(), toDsrcNic);
    }
    if (accessTechnology.test(Splitter::Interface::vlc_head)) {
        EV_INFO << "VLC head message received from upper layer!" << std::endl;

        if (draw) {
            // Won't draw at simTime() < 0.1 as TraCI is not connected and annotation fails
            auto drawCones = [this]() {
                // Headlight, right
                drawRayLine(vlcPhys[0]->getAntennaPosition(), 100, headHalfAngle);
                // left
                drawRayLine(vlcPhys[0]->getAntennaPosition(), 100, -headHalfAngle);
            };
            // The cones will be drawn immediately as a message is received from the layer above
            timerManager.create(veins::TimerSpecification(drawCones).oneshotAt(simTime()));
        }
        headlightPacketsSent += 1;
        vlcPacketsSent += 1;
        send(wsm->dup(), toVlcHead);
    }
    if (accessTechnology.test(Splitter::Interface::vlc_tail)) {
        EV_INFO << "VLC tail message received from upper layer!" << std::endl;
        if (draw) {
            auto drawCones = [this]() {
                // Taillight, left
                drawRayLine(vlcPhys[1]->getAntennaPosition(), 30, tailHalfAngle, true);
                // right
                drawRayLine(vlcPhys[1]->getAntennaPosition(), 30, -tailHalfAngle, true);
            };
            // The cones will be drawn immediately as a message is received from the layer above
            timerManager.create(veins::TimerSpecification(drawCones).oneshotAt(simTime()));
        }

        taillightPacketsSent += 1;
        vlcPacketsSent += 1;
        send(wsm->dup(), toVlcTail);
    }
}

void Splitter::handleLowerMessage(cMessage* msg)
{
    if (msg->getArrivalGateId() == fromDsrcNic) {
        headlightPacketsReceived++;
    } else if (msg->getArrivalGateId() == fromVlcHead) {
        headlightPacketsReceived += 1;
        vlcPacketsReceived += 1;
        emit(headVlcDelaySignal, simTime() - msg->getCreationTime());
        emit(totalVlcDelaySignal, simTime() - msg->getCreationTime());
    } else if (msg->getArrivalGateId() == fromVlcTail) {
        taillightPacketsReceived += 1;
        vlcPacketsReceived += 1;
        emit(tailVlcDelaySignal, simTime() - msg->getCreationTime());
        emit(totalVlcDelaySignal, simTime() - msg->getCreationTime());
    }

    send(msg, toApplication);
}

Splitter::Interfaces Splitter::getAccessTechnology(cPacket *msg) {
    if (VlcMessage* vlcMsg = dynamic_cast<VlcMessage*>(msg)) {
        return Interfaces(vlcMsg->getAccessTechnology());
    }
    else {
        return Interface::dsrc;  // fallback for non-vlc-messages
    }

}

void Splitter::drawRayLine(const AntennaPosition& ap, int length, double halfAngle, bool reverse)
{
    double heading = mobility->getHeading().getRad();
    // This is for the cone of the tail
    if (reverse) heading = reverseTraci(heading);

    annotationManager->scheduleErase(0.1,
        annotationManager->drawLine(ap.getPositionAt(),
        ap.getPositionAt() + Coord(length * cos(halfAngle + traci2myAngle(heading)), length * sin(halfAngle + traci2myAngle(heading))),
        "white"));
}

void Splitter::finish()
{
    // 'headlightPacketsSent' and 'taillightPacketsSent' will be zero if packets are not explicitly sent via them
    recordScalar("headlightPacketsSent", headlightPacketsSent);
    recordScalar("headlightPacketsReceived", headlightPacketsReceived);
    recordScalar("taillightPacketsSent", taillightPacketsSent);
    recordScalar("taillightPacketsReceived", taillightPacketsReceived);
    recordScalar("vlcPacketsSent", vlcPacketsSent);
    recordScalar("vlcPacketsReceived", vlcPacketsReceived);
}
