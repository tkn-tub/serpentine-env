//
// Copyright (C) 2016 Agon Memedi <memedi@ccs-labs.org>
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

/*
 * Based on Decider80211p.cc from David Eckhoff
 * and modifications by Bastian Bloessl, Stefan Joerer, Michele Segata, Fabian Bronner
 */

#include "veins-vlc/DeciderVlc.h"
#include "veins/modules/phy/DeciderResult80211.h"
#include "veins/base/toolbox/Signal.h"
#include "veins/modules/messages/AirFrame11p_m.h"
#include "veins/modules/utility/ConstsPhy.h"

#include "veins-vlc/messages/AirFrameVlc_m.h"
#include "veins/modules/utility/Consts80211p.h"
#include "veins-vlc/utility/Utils.h"
#include "veins/base/utils/FWMath.h"

#include "veins/base/toolbox/SignalUtils.h"

using namespace veins;

#define EV_TRACE \
    if (debug) EV_LOG(omnetpp::LOGLEVEL_TRACE, nullptr) << "[deciderVlc] "

using veins::AirFrame;

simtime_t DeciderVlc::processNewSignal(AirFrame* msg)
{

    AirFrameVlc* frame = check_and_cast<AirFrameVlc*>(msg);

    // get the receiving power of the Signal at start-time and center frequency
    Signal& signal = frame->getSignal();
    double recvPower = signal.getAtCenterFrequency();

    signalStates[frame] = EXPECT_END;

    if (signal.smallerAtCenterFrequency(minPowerLevel)) {

        EV_TRACE << "AirFrame: " << frame->getId() << " with recvPower (" << recvPower << " < " << minPowerLevel << ") -> AirFrame can't be detected by the radio; discarded at its end." << std::endl;

        // annotate the frame, so that we won't try decoding it at its end
        frame->setUnderMinPowerLevel(true);
        return signal.getReceptionEnd();
    }
    else {

        // This value might be just an intermediate result (due to short circuiting)
        setChannelIdleStatus(false);

        myBusyTime += signal.getDuration().dbl();

        if (!currentSignal.first) {
            // NIC is not yet synced to any frame, so lock and try to decode this frame
            currentSignal.first = frame;
            EV_TRACE << "AirFrame: " << frame->getId() << " with (" << recvPower << " > " << minPowerLevel << ") -> Trying to receive AirFrame." << std::endl;
        }
        else {
            // NIC is currently trying to decode another frame. this frame will be simply treated as interference
            EV_TRACE << "AirFrame: " << frame->getId() << " with (" << recvPower << " > " << minPowerLevel << ") -> Already synced to another AirFrame. Treating AirFrame as interference." << std::endl;
        }
        return signal.getReceptionEnd();
    }
}

int DeciderVlc::getSignalState(AirFrame* frame)
{

    if (signalStates.find(frame) == signalStates.end()) {
        return NEW;
    }
    else {
        return signalStates[frame];
    }
}

DeciderResult* DeciderVlc::checkIfSignalOk(AirFrame* frame)
{

    Signal& s = frame->getSignal();
    simtime_t start = s.getReceptionStart();
    simtime_t end = s.getReceptionEnd();

    // compute receive power
    double recvPower_dBm = 10 * log10(s.getAtCenterFrequency());

    start = start + PHY_VLC_SHR / bitrate; // its ok if something in the training phase is broken

    AirFrameVector airFrames;
    getChannelInfo(start, end, airFrames);

    double noise = phy->getNoiseFloorValue();

    // Make sure to use the adjusted starting-point (which ignores the preamble)
    double sinrMin = SignalUtils::getMinSINR(start, end, frame, airFrames, noise);
    double snrMin;
    if (collectCollisionStats) {
        // snrMin = SignalUtils::getMinSNR(start, end, frame, noise);
        snrMin = s.getDataMin() / noise;
    }
    else {
        // just set to any value. if collectCollisionStats != true
        // it will be ignored by packetOk
        snrMin = 1e200;
    }

    DeciderResult80211* result = 0;

    switch (packetOk(sinrMin, snrMin, frame->getBitLength())) {

    case DECODED:
        EV_TRACE << "Packet is fine! We can decode it" << std::endl;
        result = new DeciderResult80211(true, 0, sinrMin, recvPower_dBm, false);
        break;

    case NOT_DECODED:
        if (!collectCollisionStats) {
            EV_TRACE << "Packet has bit Errors. Lost " << std::endl;
        }
        else {
            EV_TRACE << "Packet has bit Errors due to low power. Lost " << std::endl;
        }
        result = new DeciderResult80211(false, 0, sinrMin, recvPower_dBm, false);
        break;

    case COLLISION:
        EV_TRACE << "Packet has bit Errors due to collision. Lost " << std::endl;
        collisions++;
        result = new DeciderResult80211(false, 0, sinrMin, recvPower_dBm, true);
        break;

    default:
        ASSERT2(false, "Impossible packet result returned by packetOk(). Check the code.");
        break;
    }

    return result;
}

enum DeciderVlc::PACKET_OK_RESULT DeciderVlc::packetOk(double sinrMin, double snrMin, int lengthMPDU)
{
    // compute success rate depending on mcs and packet length
    double packetOkSinr = getOokPdr(sinrMin, lengthMPDU); // PDR w/o FEC

    // check if header is broken
    double headerOkSinr = getOokPdr(sinrMin, PHY_VLC_SHR);

    double packetOkSnr;
    double headerOkSnr;

    // compute PER also for SNR only
    if (collectCollisionStats) {

        packetOkSnr = getOokPdr(snrMin, lengthMPDU);

        headerOkSnr = getOokPdr(snrMin, PHY_VLC_SHR);

        // the probability of correct reception without considering the interference
        // MUST be greater or equal than when consider it

        ASSERT(close(packetOkSnr, packetOkSinr) || (packetOkSnr > packetOkSinr));
        ASSERT(close(headerOkSnr, headerOkSinr) || (headerOkSnr > headerOkSinr));
    }

    // probability of no bit error in the PLCP header

    double rand = RNGCONTEXT dblrand();

    if (!collectCollisionStats) {
        if (rand > headerOkSinr) return NOT_DECODED;
    }
    else {

        if (rand > headerOkSinr) {
            // ups, we have a header error. is that due to interference?
            if (rand > headerOkSnr) {
                // no. we would have not been able to receive that even
                // without interference
                return NOT_DECODED;
            }
            else {
                // yes. we would have decoded that without interference
                return COLLISION;
            }
        }
    }

    // probability of no bit error in the rest of the packet

    rand = RNGCONTEXT dblrand();

    if (!collectCollisionStats) {
        if (rand > packetOkSinr) {
            return NOT_DECODED;
        }
        else {
            return DECODED;
        }
    }
    else {

        if (rand > packetOkSinr) {
            // ups, we have an error in the payload. is that due to interference?
            if (rand > packetOkSnr) {
                // no. we would have not been able to receive that even
                // without interference
                return NOT_DECODED;
            }
            else {
                // yes. we would have decoded that without interference
                return COLLISION;
            }
        }
        else {
            return DECODED;
        }
    }
}

simtime_t DeciderVlc::processSignalEnd(AirFrame* msg)
{

    AirFrameVlc* frame = check_and_cast<AirFrameVlc*>(msg);

    // remove this frame from our current signals
    signalStates.erase(frame);

    DeciderResult* result;

    if (frame->getUnderMinPowerLevel()) {
        // this frame was not even detected by the radio card
        result = new DeciderResult(false);
    }
    else {

        // first check whether this is the frame NIC is currently synced on
        if (frame == currentSignal.first) {
            // check if the snr is above the Decider's specific threshold,
            // i.e. the Decider has received it correctly
            result = checkIfSignalOk(frame);

            // after having tried to decode the frame, the NIC is no more synced to the frame
            // and it is ready for syncing on a new one
            currentSignal.first = 0;
        }
        else {
            // if this is not the frame we are synced on, we cannot receive it
            result = new DeciderResult(false);
        }
    }

    if (result->isSignalCorrect()) {
        EV_TRACE << "packet was received correctly, it is now handed to upper layer...\n";
        // go on with processing this AirFrame, send it to the Mac-Layer
        phy->sendUp(frame, result);
    }
    else {
        // To investigate why the packet was not received correctly
        delete result;
    }

    return notAgain;
}

void DeciderVlc::finish()
{
}

DeciderVlc::~DeciderVlc(){};
