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

#include "GeometryTest.h"

using namespace veins;

double GeometryTest::getTheta(Coord& recvPos, Coord& sendPos)
{
    Coord tx2RxVec = (recvPos - sendPos);
    double tx2Rx2D = (recvPos.atZ(0) - sendPos.atZ(0)).length();
    return atan2(tx2RxVec.z, tx2Rx2D);
}

double GeometryTest::getPhi(Coord& recvPos, Coord& sendPos, Coord& txHeadingVector)
{
    Coord tx2RxVec = (recvPos - sendPos);
    double tx2Rx2D = (recvPos.atZ(0) - sendPos.atZ(0)).length();
    Coord tx2RxNorm = tx2RxVec.atZ(0) / tx2Rx2D;
    return acos(txHeadingVector * tx2RxNorm);
}

void GeometryTest::rotatePos(Coord& C, double rotAngle, double deltaX, double deltaY, double deltaZ)
{
    C.x += (deltaX * cos(rotAngle) - deltaY * sin(rotAngle));
    C.y += (deltaX * sin(rotAngle) + deltaY * cos(rotAngle));
    C.z = deltaZ;
}

double GeometryTest::getCosIrradianceAngle(Coord& recvPos, Coord& sendPos, Coord& txHeadingVector)
{
    Coord tx2RxVec = (recvPos - sendPos);
    double tx2Rx2D = (recvPos.atZ(0) - sendPos.atZ(0)).length();
    Coord tx2RxVector = tx2RxVec.atZ(0) / tx2Rx2D;
    return utilTrunc(tx2RxVector * txHeadingVector);
}

double GeometryTest::getIrradianceAngle(double cosIrradianceAngle)
{
    return rad2deg(acos(cosIrradianceAngle));
}

double GeometryTest::getCosIncidenceAngle(Coord& recvPos, Coord& sendPos, Coord& rxHeadingVector)
{
    Coord tx2RxVec = (recvPos - sendPos);
    double tx2Rx2D = (recvPos.atZ(0) - sendPos.atZ(0)).length();
    Coord tx2RxVector = tx2RxVec.atZ(0) / tx2Rx2D;
    return utilTrunc(tx2RxVector * rxHeadingVector);
}

double GeometryTest::getIncidenceAngle(double cosIncidenceAngle)
{
    return rad2deg(acos(cosIncidenceAngle));
}

bool GeometryTest::getIfInFov(Coord& receiverPos2D, Coord& senderPos2D, Coord& txHeadingVector, double tx2RxDistance, double headlightMaxTxAngle)
{
    return ((receiverPos2D - senderPos2D) / cos(headlightMaxTxAngle)) * txHeadingVector >= tx2RxDistance;
}

bool GeometryTest::getIfOnLeft(Coord& recvPos, Coord& sendPos, Coord& txHeadingVector)
{
    Coord tx2RxVec = (recvPos - sendPos);
    double tx2Rx2D = (recvPos.atZ(0) - sendPos.atZ(0)).length();
    Coord tx2RxNorm = tx2RxVec.atZ(0) / tx2Rx2D;
    return txHeadingVector.twoDimensionalCrossProduct(tx2RxNorm) < 0;
}

bool GeometryTest::unitVecNegative(Coord& recvPos, Coord& sendPos, Coord& rxHeadingVector)
{
    Coord tx2RxVec = (recvPos - sendPos);
    double tx2Rx2D = (recvPos.atZ(0) - sendPos.atZ(0)).length();
    Coord tx2RxNorm = tx2RxVec.atZ(0) / tx2Rx2D;
    return tx2RxNorm.x < 0;
}

double GeometryTest::getAcosBearing(Coord& recvPos, Coord& sendPos, Coord& rxHeadingVector)
{
    Coord tx2RxVec = (recvPos - sendPos);
    double tx2Rx2D = (recvPos.atZ(0) - sendPos.atZ(0)).length();
    Coord tx2RxNorm = tx2RxVec.atZ(0) / tx2Rx2D;
    return (tx2RxNorm * rxHeadingVector);
}

bool GeometryTest::getIfInBearing(Coord& recvPos, Coord& sendPos, Coord& rxHeadingVector)
{
    Coord tx2RxVec = (recvPos - sendPos);
    double tx2Rx2D = (recvPos.atZ(0) - sendPos.atZ(0)).length();
    Coord tx2RxNorm = tx2RxVec.atZ(0) / tx2Rx2D;
    return (tx2RxNorm * rxHeadingVector) < 0;
}
