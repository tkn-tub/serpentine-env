
//
// Copyright (C) 2018-2018 Julien Jahneke <julien.jahneke@ccs-labs.org>
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

#include "veins-vlc/veins-vlc.h"

#include "veins/base/utils/Coord.h"
#include "veins-vlc/utility/ConstsVlc.h"
#include "veins-vlc/utility/Utils.h"

namespace veins {

/**
 * @brief This is a dummy class to test geometry calculations of the
 * EmpiricalLightModel and the LsvLightModel
 *
 */

class VEINS_VLC_API GeometryTest {
public:
    GeometryTest(){};

    ~GeometryTest(){};

    double getTheta(Coord& recvPos, Coord& sendPos);
    double getPhi(Coord& recvPos, Coord& sendPos, Coord& txHeadingVector);
    void rotatePos(Coord& C, double rotAngle, double deltaX, double deltaY, double deltaZ);
    double getCosIrradianceAngle(Coord& rxPos, Coord& txPos, Coord& txHeadingVector);
    double getIrradianceAngle(double cosIrradianceAngle);
    double getCosIncidenceAngle(Coord& rxPos, Coord& txPos, Coord& rxHeadingVector);
    double getIncidenceAngle(double cosIncidenceAngle);
    bool getIfOnLeft(Coord& recvPos, Coord& sendPos, Coord& txHeadingVector);
    bool getIfInFov(Coord& receiverPos2D, Coord& senderPos2D, Coord& txHeadingVector, double tx2RxDistance, double headlightMaxTxAngle);
    bool getIfInBearing(Coord& recvPos, Coord& sendPos, Coord& rxHeadingVector);
    double getAcosBearing(Coord& recvPos, Coord& sendPos, Coord& rxHeadingVector);
    bool unitVecNegative(Coord& recvPos, Coord& sendPos, Coord& rxHeadingVector);
};
} // namespace veins
