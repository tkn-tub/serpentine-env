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

#pragma once

#include "veins/base/phyLayer/BasePhyLayer.h"
#include "veins/base/toolbox/Spectrum.h"
#include "veins/modules/mac/ieee80211p/Mac80211pToPhy11pInterface.h"
#include "veins-vlc/DeciderVlc.h"
#include "veins/modules/analogueModel/SimplePathlossModel.h"
#include "veins/base/connectionManager/BaseConnectionManager.h"
#include "veins/modules/phy/Decider80211pToPhy80211pInterface.h"
#include "veins/base/utils/Move.h"

#include "veins-vlc/analogueModel/EmpiricalLightModel.h"
#include "veins-vlc/utility/ConstsVlc.h"

#include "veins-vlc/analogueModel/LsvLightModel.h"
#include "veins-vlc/RadiationPattern.h"
#include "veins-vlc/Photodiode.h"

namespace veins {

/**
 * @brief
 * Adaptation of the PhyLayer class for 802.11p.
 *
 * @ingroup phyLayer
 *
 * @see DemoBaseApplLayer
 * @see Mac1609_4
 * @see PhyLayer80211p
 * @see Decider80211p
 */

class PhyLayerVlc : public BasePhyLayer {
public:
    void initialize(int stage) override;

    static bool mapsInitialized;
    static std::map<std::string, RadiationPattern> radiationPatternMap;
    static std::map<std::string, Photodiode> photodiodeMap;

protected:
    /** @brief enable/disable detection of packet collisions */
    bool collectCollisionStatistics;

    /** @brief The power (in mW) to transmit with.*/
    double txPower;

    double bitrate;

    enum ProtocolIds {
        VLC = 12124
    };
    /**
     * @brief Creates and returns an instance of the AnalogueModel with the
     * specified name.
     *
     * Is able to initialize the following AnalogueModels:
     */
    virtual std::unique_ptr<AnalogueModel> getAnalogueModelFromName(std::string name, ParameterMap& params) override;

    /**
     * @brief Creates and initializes a VehicleObstacleShadowing with the
     * passed parameter values.
     */
    std::unique_ptr<AnalogueModel> initializeVehicleObstacleShadowingForVlc(ParameterMap& params);

    /**
     * @brief Creates and initializes a EmpiricalLightModel with the
     * passed parameter values.
     */
    std::unique_ptr<AnalogueModel> initializeEmpiricalLightModel(ParameterMap& params);

    /**
     * @brief Creates and initializes a LsvLightModel with the
     * passed parameter values.
     */
    std::unique_ptr<AnalogueModel> initializeLsvLightModel(ParameterMap& params);

    /**
     * Create and return an instance of the Antenna with the specified name as a shared pointer.
     *
     * This method is called during initialization to load the Antenna specified.
     * If no special antenna has been specified, an object of the base Antenna class is instantiated, representing an isotropic antenna.
     */
    std::shared_ptr<Antenna> getAntennaFromName(std::string name, ParameterMap& params) override;

    /**
     * @brief Creates and returns an instance of the Decider with the specified
     * name.
     *
     * Is able to initialize the following Deciders:
     *
     * - DeciderVlc
     */
    virtual std::unique_ptr<Decider> getDeciderFromName(std::string name, ParameterMap& params) override;

    /**
     * @brief Initializes a new Decider80211 from the passed parameter map.
     */
    virtual std::unique_ptr<Decider> initializeDeciderVlc(ParameterMap& params);

    /**
     * Create a protocol-specific AirFrame
     * Overloaded to create a specialize AirFrameVlc.
     */
    std::unique_ptr<AirFrame> createAirFrame(cPacket* macPkt) override;

    /**
     * @brief This function encapsulates messages from the upper layer into an
     * AirFrame and sets all necessary attributes.
     */
    virtual std::unique_ptr<AirFrame> encapsMsg(cPacket* msg) override;

    simtime_t getFrameDuration(int payloadLengthBits) const;

    virtual void handleMessage(cMessage* msg) override;
    simtime_t setRadioState(int rs) override;

    std::shared_ptr<Antenna> initializeAntennaHeadlight(ParameterMap& params);
    std::shared_ptr<Antenna> initializeAntennaTaillight(ParameterMap& params);
};

} // namespace veins
