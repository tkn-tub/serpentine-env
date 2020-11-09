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

/*
 * Based on PhyLayer80211p.cc from David Eckhoff
 */

#include "veins-vlc/PhyLayerVlc.h"

#include "veins-vlc/DeciderVlc.h"
#include "veins-vlc/analogueModel/VehicleObstacleShadowingForVlc.h"
#include "veins/base/connectionManager/BaseConnectionManager.h"
#include "veins/modules/messages/AirFrame11p_m.h"
#include "veins-vlc/messages/AirFrameVlc_m.h"
#include "veins-vlc/AntennaHeadlight.h"
#include "veins-vlc/AntennaTaillight.h"

using namespace veins;

using std::unique_ptr;

Define_Module(veins::PhyLayerVlc);

/* Used for the LsvLightModel */
bool PhyLayerVlc::mapsInitialized = false;
std::map<std::string, Photodiode> PhyLayerVlc::photodiodeMap = std::map<std::string, Photodiode>();
std::map<std::string, RadiationPattern> PhyLayerVlc::radiationPatternMap = std::map<std::string, RadiationPattern>();

void PhyLayerVlc::initialize(int stage)
{
    if (stage == 0) {
        txPower = par("txPower").doubleValue();
        if (txPower != FIXED_REFERENCE_POWER_MW)
            error("You have set the wrong reference power (txPower) in omnetpp.ini. Should be fixed to: FIXED_REFERENCE_POWER");
        bitrate = par("bitrate").doubleValue();
        collectCollisionStatistics = par("collectCollisionStatistics").boolValue();

        // Create frequency mappings and initialize spectrum for signal representation
        overallSpectrum = Spectrum({666e12});
    }
    BasePhyLayer::initialize(stage);
}

unique_ptr<AnalogueModel> PhyLayerVlc::getAnalogueModelFromName(std::string name, ParameterMap& params)
{

    if (name == "EmpiricalLightModel") {
        return initializeEmpiricalLightModel(params);
    }
    else if (name == "LsvLightModel") {
        return initializeLsvLightModel(params);
    }
    else if (name == "VehicleObstacleShadowingForVlc") {
        return initializeVehicleObstacleShadowingForVlc(params);
    }
    return BasePhyLayer::getAnalogueModelFromName(name, params);
}

unique_ptr<AnalogueModel> PhyLayerVlc::initializeEmpiricalLightModel(ParameterMap& params)
{

    double headlightMaxTxRange = 0.0, taillightMaxTxRange = 0.0, headlightMaxTxAngle = 0.0, taillightMaxTxAngle = 0.0;
    ParameterMap::iterator it;

    it = params.find("headlightMaxTxRange");
    if (it != params.end()) {
        headlightMaxTxRange = it->second.doubleValue();
    }
    else {
        error("`headlightMaxTxRange` has not been specified in config-vlc.xml");
    }

    it = params.find("taillightMaxTxRange");
    if (it != params.end()) {
        taillightMaxTxRange = it->second.doubleValue();
    }
    else {
        error("`taillightMaxTxRange` has not been specified in config-vlc.xml");
    }

    it = params.find("headlightMaxTxAngle");
    if (it != params.end()) {
        headlightMaxTxAngle = it->second.doubleValue();
    }
    else {
        error("`headlightMaxTxAngle` has not been specified in config-vlc.xml");
    }

    it = params.find("taillightMaxTxAngle");
    if (it != params.end()) {
        taillightMaxTxAngle = it->second.doubleValue();
    }
    else {
        error("`taillightMaxTxAngle` has not been specified in config-vlc.xml");
    }

    return make_unique<EmpiricalLightModel>(this, FWMath::mW2dBm(minPowerLevel), headlightMaxTxRange, taillightMaxTxRange, headlightMaxTxAngle, taillightMaxTxAngle);
}

// version using line-by-line parsing
unique_ptr<AnalogueModel> PhyLayerVlc::initializeLsvLightModel(ParameterMap& params)
{
    if (mapsInitialized == false) {

        ParameterMap::iterator it;
        std::string radiationPatternFile;
        std::string photodiodeFile;

        it = params.find("radiationPatternFile");
        if (it != params.end()) {
            radiationPatternFile = it->second.stringValue();
        }
        else {
            error("`radiationPatternFile` has not been specified in config-vlc-lsv.xml");
        }

        it = params.find("photodiodeFile");
        if (it != params.end()) {
            photodiodeFile = it->second.stringValue();
        }
        else {
            error("`photodiodeFile` has not been specified in config-vlc-lsv.xml");
        }

        // For file parsing
        std::ifstream inputFile(radiationPatternFile);
        std::string line;
        int lineCounter;
        double value;

        // For radiation pattern
        std::string Id;
        std::vector<double> patternL, patternR, anglesL, anglesR, spectralEmission;

        lineCounter = 0;
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            switch (lineCounter) {
            case 0:
                iss >> Id;
                ++lineCounter;
                break;
            case 1:
                while (iss >> value) patternL.push_back(value);
                ++lineCounter;
                break;
            case 2:
                while (iss >> value) patternR.push_back(value);
                ++lineCounter;
                break;
            case 3:
                while (iss >> value) anglesL.push_back(value);
                ++lineCounter;
                break;
            case 4:
                while (iss >> value) anglesR.push_back(value);
                ++lineCounter;
                break;
            case 5:
                while (iss >> value) spectralEmission.push_back(value);
                lineCounter = 0;
                radiationPatternMap.insert(std::pair<std::string, RadiationPattern>(Id, RadiationPattern(Id, patternL, patternR, anglesL, anglesR, spectralEmission)));
                // Clear all vectors for next pattern
                patternL.clear();
                patternR.clear();
                anglesL.clear();
                anglesR.clear();
                spectralEmission.clear();
                break;
            }
            iss.str("");
        }

        // For file parsing
        std::ifstream inputFile2(photodiodeFile);

        // For photodiode
        double area, gain;
        std::vector<double> spectralResponse;

        lineCounter = 0;
        while (std::getline(inputFile2, line)) {
            std::istringstream iss(line);
            switch (lineCounter) {
            case 0:
                iss >> Id;
                ++lineCounter;
                break;
            case 1:
                iss >> area;
                ++lineCounter;
                break;
            case 2:
                iss >> gain;
                ++lineCounter;
                break;
            case 3:
                while (iss >> value) spectralResponse.push_back(value);
                lineCounter = 0;
                photodiodeMap.insert(std::pair<std::string, Photodiode>(Id, Photodiode(Id, area, gain, spectralResponse)));
                spectralResponse.clear();
                break;
            }
            iss.str("");
        }
        mapsInitialized = true;
    }
    return make_unique<LsvLightModel>(this, &radiationPatternMap, &photodiodeMap, FWMath::mW2dBm(minPowerLevel));
}

unique_ptr<Decider> PhyLayerVlc::getDeciderFromName(std::string name, ParameterMap& params)
{
    if (name == "DeciderVlc") {
        protocolId = VLC;
        return initializeDeciderVlc(params);
    }
    return BasePhyLayer::getDeciderFromName(name, params);
}

std::shared_ptr<Antenna> PhyLayerVlc::getAntennaFromName(std::string name, ParameterMap& params)
{
    // If a headlight phy initialize "HeadlightAntenna"
    if (name == "HeadlightAntenna") {
        return initializeAntennaHeadlight(params);
    }

    // If a taillight phy initialize "TaillightAntenna")
    if (name == "TaillightAntenna") {
        return initializeAntennaTaillight(params);
    }

    error("PhyLayerVlc only knows about HeadlightAntenna and TaillightAntenna");

    return nullptr;
}

std::shared_ptr<Antenna> PhyLayerVlc::initializeAntennaHeadlight(ParameterMap& params)
{
    double photodiodeGroundOffsetZ = par("photodiodeGroundOffsetZ");
    double interModuleDistance = par("interModuleDistance");
    std::string radiationPatternId = par("radiationPatternId");
    std::string photodiodeId = par("photodiodeId");

    return std::make_shared<AntennaHeadlight>(photodiodeGroundOffsetZ, interModuleDistance, radiationPatternId, photodiodeId);
}

std::shared_ptr<Antenna> PhyLayerVlc::initializeAntennaTaillight(ParameterMap& params)
{
    double photodiodeGroundOffsetZ = par("photodiodeGroundOffsetZ");
    double interModuleDistance = par("interModuleDistance");
    std::string radiationPatternId = par("radiationPatternId");
    std::string photodiodeId = par("photodiodeId");

    return std::make_shared<AntennaTaillight>(photodiodeGroundOffsetZ, interModuleDistance, radiationPatternId, photodiodeId);
}

unique_ptr<AnalogueModel> PhyLayerVlc::initializeVehicleObstacleShadowingForVlc(ParameterMap& params)
{
    // init with default value
    bool useTorus = world->useTorus();
    const Coord& playgroundSize = *(world->getPgs());

    VehicleObstacleControl* vehicleObstacleControlP = VehicleObstacleControlAccess().getIfExists();
    if (!vehicleObstacleControlP) throw cRuntimeError("initializeVehicleObstacleShadowingForVlc(): cannot find VehicleObstacleControl module");
    return make_unique<VehicleObstacleShadowingForVlc>(this, *vehicleObstacleControlP, useTorus, playgroundSize);
}

unique_ptr<Decider> PhyLayerVlc::initializeDeciderVlc(ParameterMap& params)
{
    DeciderVlc* dec = new DeciderVlc(this, this, minPowerLevel, bitrate, findHost()->getIndex(), collectCollisionStatistics);
    return unique_ptr<DeciderVlc>(std::move(dec));
}

void PhyLayerVlc::handleMessage(cMessage* msg)
{
    // self messages
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);

        // MacPkts <- MacToPhyControlInfo
    }
    else if (msg->getArrivalGateId() == upperLayerIn) {
        setRadioState(veins::Radio::TX);
        BasePhyLayer::handleUpperMessage(msg);

        // controlmessages
    }
    else if (msg->getArrivalGateId() == upperControlIn) {
        BasePhyLayer::handleUpperControlMessage(msg);

        // AirFrames
        // msg received over air from other NICs
    }
    else if (msg->getKind() == AIR_FRAME) {
        AirFrameVlc* VlcMsg = check_and_cast<AirFrameVlc*>(msg);
        std::string txNode = VlcMsg->getSenderModule()->getModuleByPath("^.^.")->getFullName();
        if (txNode == getModuleByPath("^.^.")->getFullName()) {
            EV_TRACE << "Discarding received AirFrameVlc within the same host: " << VlcMsg->getSenderModule()->getFullPath() << std::endl;
            delete msg;
            return;
        }
        else {
            EV_TRACE << "AirFrameVlc id: " << VlcMsg->getId() << " handed to VLC PHY from: " << VlcMsg->getSenderModule()->getFullPath() << std::endl;
            bubble("Handing AirFrameVlc to lower layers to decide if it can be received");
            BasePhyLayer::handleAirFrame(static_cast<AirFrame*>(msg));
        }
    }
    else {
        EV_TRACE << "Unknown message received." << endl;
        delete msg;
    }
}

unique_ptr<AirFrame> PhyLayerVlc::createAirFrame(cPacket* macPkt)
{
    return make_unique<AirFrameVlc>(macPkt->getName(), AIR_FRAME);
}

unique_ptr<AirFrame> PhyLayerVlc::encapsMsg(cPacket* macPkt)
{
    macPkt->addBitLength(PHY_VLC_HEADER);

    auto airFrame = createAirFrame(macPkt);
    AirFrameVlc* frame = dynamic_cast<AirFrameVlc*>(airFrame.get());

    // set the members
    // set priority of AirFrames above the normal priority to ensure
    // channel consistency (before any thing else happens at a time
    // point t make sure that the channel has removed every AirFrame
    // ended at t and added every AirFrame started at t)
    frame->setSchedulingPriority(airFramePriority());
    frame->setProtocolId(myProtocolId());
    frame->setId(world->getUniqueAirFrameId());
    frame->setChannel(radio->getCurrentChannel());

    // encapsulate the mac packet into the phy frame
    frame->encapsulate(macPkt);

    // attachSignal()
    // attach the spectrum-dependent Signal to the airFrame
    const auto duration = getFrameDuration(frame->getEncapsulatedPacket()->getBitLength());
    ASSERT(duration > 0);
    Signal signal(overallSpectrum, simTime(), duration);
    signal.at(0) = txPower;
    signal.setDataStart(0);
    signal.setDataEnd(0);
    signal.setCenterFrequencyIndex(0);
    // copy the signal into the AirFrame
    frame->setSignal(signal);
    frame->setDuration(signal.getDuration());
    // --- end of attachSignal() ---

    // --- from here on, the AirFrame is the owner of the MacPacket ---
    macPkt = nullptr;
    EV_TRACE << "AirFrame w/ id: " << frame->getId() << " encapsulated, bit length: " << frame->getBitLength() << "\n";

    return airFrame;
}

simtime_t PhyLayerVlc::setRadioState(int rs)
{
    if (rs == Radio::TX) decider->switchToTx();
    return BasePhyLayer::setRadioState(rs);
}

simtime_t PhyLayerVlc::getFrameDuration(int payloadLengthBits) const
{
    // Following assumptions apply:
    // i) The SHR and the HEADER are sent with the same bitrate as the payload
    // ii) Due to OOK, the number of bits per symbol (n_nbps) == 1, so the payload is not divided
    simtime_t duration = (PHY_VLC_SHR + PHY_VLC_HEADER) / bitrate + payloadLengthBits / bitrate;
    return duration;
}
