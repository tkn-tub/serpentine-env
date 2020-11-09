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

// Based on previous implementation from Hua-Yen Tseng

#pragma once

#include <cassert>

#include "veins-vlc/veins-vlc.h"

#include "veins/base/phyLayer/AnalogueModel.h"
#include "veins-vlc/utility/ConstsVlc.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/world/annotations/AnnotationManager.h"
#include "veins-vlc/utility/Utils.h"
#include "veins/base/utils/POA.h"

using veins::AirFrame;
using veins::AnnotationManager;
using veins::AnnotationManagerAccess;
using veins::TraCIMobility;

namespace veins {

#define ELMDEBUG(word)                                 \
    if (debug) {                                       \
        std::cout << "\t\tELM: " << word << std::endl; \
    }

/**
 * @brief This class returns the received power in dbm on the
 * receiving module, based on the empirical measurements
 * with taillight and headlight
 *
 * The values are recorded as observed from the spectrum analyzer
 * to which the PD is connected -- this is electrical power
 */
class VEINS_VLC_API EmpiricalLightModel : public AnalogueModel {
protected:
    AnnotationManager* annotations;

    bool debug = false;
    double sensitivity_dbm;
    double headlightMaxTxRange;
    double taillightMaxTxRange;
    double headlightMaxTxAngle;
    double taillightMaxTxAngle;

public:
    EmpiricalLightModel(cComponent* owner, double rxSensitivity_dbm, double m_headlightMaxTxRange, double m_taillightMaxTxRange, double m_headlightMaxTxAngle, double m_taillightMaxTxAngle)
        : AnalogueModel(owner)
        , sensitivity_dbm(rxSensitivity_dbm)
        , headlightMaxTxRange(m_headlightMaxTxRange)
        , taillightMaxTxRange(m_taillightMaxTxRange)
        , headlightMaxTxAngle(m_headlightMaxTxAngle)
        , taillightMaxTxAngle(m_taillightMaxTxAngle)
    {
        // Transform degrees into radians
        headlightMaxTxAngle = deg2rad(headlightMaxTxAngle);
        taillightMaxTxAngle = deg2rad(taillightMaxTxAngle);

        annotations = AnnotationManagerAccess().getIfExists();
        ASSERT(annotations);
    };

    void filterSignal(Signal*) override;

    int getLightingModuleOrientation(POA poa);

    bool isRecvPowerUnderSensitivity(int senderHeading, double distanceFromSenderToReceiver, const Coord& vectorFromTx2Rx, const Coord& vectorTxHeading, const Coord& vectorRxHeading);
    double calcReceivedPower(int senderHeading, double distanceFromSenderToReceiver, const Coord& vectorFromTx2Rx, const Coord& vectorTxHeading, const Coord& vectorRxHeading);
    double calcFittedReceivedPower(double distanceFromSenderToReceiver, const Coord& vectorFromTx2Rx, const Coord& vectorTxHeading);
};

} // namespace veins
