//
// Copyright (C) 2018 Julien Jahneke <julien.jahneke@ccs-labs.org>
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

#include <limits>

#include <veins-vlc/analogueModel/LsvLightModel.h>

#include "veins/base/messages/AirFrame_m.h"
#include "veins-vlc/messages/AirFrameVlc_m.h"
#include "veins-vlc/AntennaHeadlight.h"
#include "veins-vlc/AntennaTaillight.h"

using namespace veins;

#define EV_TRACE \
    if (debug) EV_LOG(omnetpp::LOGLEVEL_TRACE, nullptr) << "[lsvLightModel] "

// Returns true in case the given angles are within the bounds set by RP
bool LsvLightModel::inFOV(int LeftOrRight, double irradianceTheta, double irradiancePhi)
{
    irradiancePhi = rad2deg(irradiancePhi);
    irradianceTheta = rad2deg(irradianceTheta);
    bool inFovV = false;
    bool inFovH = false;

    if (LeftOrRight == LEFT) {
        inFovH = (irradiancePhi >= RP->getAnglesLeftFromIndex(0)) && (irradiancePhi <= RP->getAnglesLeftFromIndex(1));
        inFovV = (irradianceTheta >= RP->getAnglesLeftFromIndex(3)) && (irradianceTheta <= RP->getAnglesLeftFromIndex(4));
    }
    else if (LeftOrRight == RIGHT) {
        inFovH = (irradiancePhi >= RP->getAnglesRightFromIndex(0)) && (irradiancePhi <= RP->getAnglesRightFromIndex(1));
        inFovV = (irradianceTheta >= RP->getAnglesRightFromIndex(3)) && (irradianceTheta <= RP->getAnglesRightFromIndex(4));
    }

    return (inFovV && inFovH);
}

// Return value from radiation pattern matrix corresponding to angles of emission
double LsvLightModel::getFromMatrix(int LeftOrRight, double irradianceTheta, double irradiancePhi)
{
    int indexPhi, indexTheta, rowLength, indexFlat;
    double value = 0;
    if (LeftOrRight == LEFT) {
        indexPhi = int(round((rad2deg(irradiancePhi) - RP->getAnglesLeftFromIndex(0)) / RP->getAnglesLeftFromIndex(2)));
        indexTheta = int(round((rad2deg(irradianceTheta) - RP->getAnglesLeftFromIndex(3)) / RP->getAnglesLeftFromIndex(5)));
        rowLength = int(round((RP->getAnglesLeftFromIndex(1) - RP->getAnglesLeftFromIndex(0) + RP->getAnglesLeftFromIndex(2)) / RP->getAnglesLeftFromIndex(2)));
        indexFlat = (rowLength * indexTheta) + indexPhi;
        value = RP->getPatternLeftFromIndex(indexFlat);
    }
    else if (LeftOrRight == RIGHT) {
        indexPhi = int(round((rad2deg(irradiancePhi) - RP->getAnglesRightFromIndex(0)) / RP->getAnglesRightFromIndex(2)));
        indexTheta = int(round((rad2deg(irradianceTheta) - RP->getAnglesRightFromIndex(3)) / RP->getAnglesRightFromIndex(5)));
        rowLength = int(round((RP->getAnglesRightFromIndex(1) - RP->getAnglesRightFromIndex(0) + RP->getAnglesRightFromIndex(2)) / RP->getAnglesRightFromIndex(2)));
        indexFlat = (rowLength * indexTheta) + indexPhi;
        value = RP->getPatternRightFromIndex(indexFlat);
    }
    return value;
}

// Calculate position of module with given offsets
void LsvLightModel::rotatePos(Coord& C, double rotAngle, double deltaX, double deltaY, double deltaZ)
{
    C.x += (deltaX * cos(rotAngle) - deltaY * sin(rotAngle));
    C.y += (deltaX * sin(rotAngle) + deltaY * cos(rotAngle));
    C.z = deltaZ;

    EV_TRACE << "Rotate " << C.info() << "by: " << rad2deg(rotAngle)
        << "deg with offsets: (" << deltaX << ", "
        << deltaY << ", " << deltaZ << ")"
        << "\tResulting position: " << C.info() << std::endl;
}

// Return optical power at photodiode surface
double LsvLightModel::getOpticalPower(double irradiance, double incidenceTheta, double incidencePhi)
{
    double area = PD->getArea();
    double areaTiltedDiode = area * cos(incidenceTheta) * cos(incidencePhi);
    return irradiance * areaTiltedDiode;
}

// Return received electrical power in mW
double LsvLightModel::getElectricalPowermW(double opticalPower)
{
    double gain = PD->getGain();
    double currentFactor = getCurrentFactor();
    double powerRxElW = pow((opticalPower * currentFactor * gain), 2) / 50;
    return (powerRxElW * 1000);
}

// Return the average photo-current produced by the photo-diode
double LsvLightModel::getCurrentFactor()
{
    std::vector<double> emission = RP->getSpectralEmission();
    std::vector<double> response = PD->getSpectralResponse();
    if (emission.size() != response.size()) {
        throw cRuntimeError("Spectral emission and spectral response vectors are not of same size!");
    }

    double sumEmission = 0;
    double sumEmissionResponse = 0;
    for (size_t i = 0; i < emission.size(); ++i) {
        sumEmission += emission[i];
        sumEmissionResponse += emission[i] * response[i];
    }
    double factor = sumEmissionResponse / sumEmission;
    return factor;
}

// Returns pointer to RadiationPattern that matches to key in map
RadiationPattern* LsvLightModel::getRadiationPatternFromKey(std::string key)
{
    auto it = RP_Map->find(key);
    if (it == RP_Map->end()) {
        throw cRuntimeError("No id matching %s found in list of radiation patterns", key.c_str());
    }
    return &(it->second);
}

// Returns pointer to Photodiode that matches to key in map
Photodiode* LsvLightModel::getPhotodiodeFromKey(std::string key)
{
    auto it = PD_Map->find(key);
    if (it == PD_Map->end()) {
        throw cRuntimeError("No id matching %s found in list of photodiodes", key.c_str());
    }
    return &(it->second);
}

void LsvLightModel::filterSignal(Signal* signal)
{
    auto sender = signal->getSenderPoa();
    auto receiver = signal->getReceiverPoa();

    auto senderPos = sender.pos.getPositionAt();

    auto* senderAntenna = dynamic_cast<AntennaVlc*>(sender.antenna.get());
    auto* receiverAntenna = dynamic_cast<AntennaVlc*>(receiver.antenna.get());

    std::string keyRadiationPattern(senderAntenna->radiationPatternId);
    std::string keyPhotodiode(receiverAntenna->photodiodeId);
    RP = getRadiationPatternFromKey(keyRadiationPattern);
    PD = getPhotodiodeFromKey(keyPhotodiode);

    EV_TRACE << "Tx Radiation Pattern: " << keyRadiationPattern << "\tRx Photodiode: " << keyPhotodiode << std::endl;

    // Get direction angle
    double txHeadingAngle = traci2myAngle(Heading::fromCoord(sender.orientation).getRad());
    double rxHeadingAngle = traci2myAngle(Heading::fromCoord(receiver.orientation).getRad());

    // Get if front or rear headlight
    int txOrientation = getLightingModuleOrientation(sender);
    int rxOrientation = getLightingModuleOrientation(receiver);

    EV_TRACE << "txHeading: " << txHeadingAngle
        << "\trxHeading: " << rxHeadingAngle
        << "\ttxHeading (deg): " << rad2deg(txHeadingAngle)
        << "\trxHeading (deg): " << rad2deg(rxHeadingAngle) << std::endl;

    // Vectors pointing in direction of travel/face
    Coord txHeadingVector = Coord(cos(txHeadingAngle), sin(txHeadingAngle)) * txOrientation;
    Coord rxHeadingVector = Coord(cos(rxHeadingAngle), sin(rxHeadingAngle)) * rxOrientation;

    Coord sendPos_L = sender.pos.getPositionAt();
    Coord sendPos_R = sender.pos.getPositionAt();
    Coord recvPos = receiver.pos.getPositionAt();
    double interModuleDist = senderAntenna->interModuleDistance;
    rotatePos(sendPos_L, txHeadingAngle, 0, -interModuleDist / 2, senderPos.z);
    rotatePos(sendPos_R, txHeadingAngle, 0, interModuleDist / 2, senderPos.z);
    double photodiodeHeight = receiverAntenna->photodiodeGroundOffsetZ;
    rotatePos(recvPos, rxHeadingAngle, 0, 0, photodiodeHeight);

    // Calculate 2D distance between points
    Coord tx2RxVec_L = (recvPos - sendPos_L);
    double tx2Rx2D_L = (recvPos.atZ(0) - sendPos_L.atZ(0)).length();
    Coord tx2RxNorm_L = tx2RxVec_L.atZ(0) / tx2Rx2D_L;
    double irradianceTheta_L = atan2(tx2RxVec_L.z, tx2Rx2D_L);
    double irradiancePhi_L = acos(txHeadingVector * tx2RxNorm_L);

    double recvPowermW_L = 0;
    // Is the receiver left of the module
    if (txHeadingVector.twoDimensionalCrossProduct(tx2RxNorm_L) < 0) {
        irradiancePhi_L *= -1;
    }

    // Calculate if in FOV
    bool inFov_L = inFOV(LEFT, irradianceTheta_L, irradiancePhi_L);
    if (!inFov_L) {
        EV_TRACE << "Not in field-of-view of left light module" << std::endl;
    }
    else {
        EV_TRACE << "In field-of-view of left light module" << std::endl;
    }

    double cosIncidenceAngle_L = tx2RxNorm_L * rxHeadingVector;
    bool inTxBearing_L = cosIncidenceAngle_L < 0;
    if (!inTxBearing_L) {
        EV_TRACE << "Not in bearing" << std::endl;
    }
    else {
        EV_TRACE << "In bearing" << std::endl;
    }
    if (inFov_L && inTxBearing_L) {
        // Could be calculated by acos(rxHeading * rx2TxNorm);
        //        incidencePhi_L = acos(rxHeadingVector * (-1 * tx2RxNorm_L));
        double incidencePhi_L = acos((tx2RxNorm_L * rxHeadingVector) * (-1));

        // Calculate power
        double matrixValue_L = getFromMatrix(LEFT, irradianceTheta_L, irradiancePhi_L);
        double irradianceAtRecv_L = matrixValue_L / tx2RxVec_L.length();
        double opticalPower_L = getOpticalPower(irradianceAtRecv_L, irradianceTheta_L, incidencePhi_L);
        recvPowermW_L = getElectricalPowermW(opticalPower_L);
    }
    else {
        EV_TRACE << "Message cannot be received from left light module" << std::endl;
    }

    // --------------------------------------------------------------------//
    //                  Right side module                                 //
    // --------------------------------------------------------------------//
    // Same calculations for right light module
    Coord tx2RxVec_R = (recvPos - sendPos_R);
    double tx2Rx2D_R = (recvPos.atZ(0) - sendPos_R.atZ(0)).length();
    Coord tx2RxNorm_R = tx2RxVec_R.atZ(0) / tx2Rx2D_R;
    double irradianceTheta_R = atan2(tx2RxVec_R.z, tx2Rx2D_R);
    double irradiancePhi_R = acos(txHeadingVector * tx2RxNorm_R);

    double recvPowermW_R = 0;
    // Is the receiver left of the module
    if (txHeadingVector.twoDimensionalCrossProduct(tx2RxNorm_R) < 0) {
        irradiancePhi_R *= -1;
    }

    // Calculate if in FOV
    double inFov_R = inFOV(RIGHT, irradianceTheta_R, irradiancePhi_R);
    if (!inFov_R) {
        EV_TRACE << "Not in field-of-view of right light module" << std::endl;
    }
    else {
        EV_TRACE << "In field-of-view of right light module" << std::endl;
    }

    double cosIncidenceAngle_R = tx2RxNorm_R * rxHeadingVector;
    bool inTxBearing_R = cosIncidenceAngle_R < 0;
    if (!inTxBearing_R) {
        EV_TRACE << "Not in bearing" << std::endl;
    }
    else {
        EV_TRACE << "In bearing" << std::endl;
    }
    if (inFov_R && inTxBearing_R) {
        // Could be calculated by acos(rxHeading * rx2TxNorm);
        //        incidencePhi_R = acos(rxHeadingVector * (-1 * tx2RxNorm_R));
        double incidencePhi_R = acos((tx2RxNorm_R * rxHeadingVector) * (-1));

        // Calculate power
        double matrixValue_R = getFromMatrix(RIGHT, irradianceTheta_R, irradiancePhi_R);
        double irradianceAtRecv_R = matrixValue_R / tx2RxVec_R.length();
        double opticalPower_R = getOpticalPower(irradianceAtRecv_R, irradianceTheta_R, incidencePhi_R);
        recvPowermW_R = getElectricalPowermW(opticalPower_R);
    }
    else {
        EV_TRACE << "Message cannot be received from right light module" << std::endl;
    }

    // --------------------------------------------------------------------//

    // Calculations complete, set signal properties
    double recvPowermW = recvPowermW_L + recvPowermW_R;
    double recvPower_dbm = sensitivity_dbm;
    double attenuationFactor = 0;
    if (recvPowermW > 0) {
        recvPower_dbm = FWMath::mW2dBm(recvPowermW);
        EV_TRACE << "Received power: \t"
            << recvPowermW << " mW\t"
            << recvPower_dbm << "dBm\t" << std::endl;
    }
    else {
        EV_TRACE << "No signal power received" << std::endl;
    }
    if (recvPower_dbm > sensitivity_dbm) {
        attenuationFactor = recvPowermW / FIXED_REFERENCE_POWER_MW;
    }

    *signal *= attenuationFactor;
}

int LsvLightModel::getLightingModuleOrientation(POA poa)
{
    if (dynamic_cast<AntennaHeadlight*>(poa.antenna.get())) {
        return HEAD;
    }
    if (dynamic_cast<AntennaTaillight*>(poa.antenna.get())) {
        return TAIL;
    }
    throw cRuntimeError("EmpiricalLightModel only handles transmissions by AntennaHeadlight or AntennaTaillight");
}
