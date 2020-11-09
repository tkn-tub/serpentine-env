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

#include "veins-vlc/utility/Utils.h"

using namespace veins;

namespace veins {

double traci2myAngle(double angleRad)
{
    if (angleRad >= -M_PI && angleRad < 0)
        angleRad = -angleRad;
    else if (angleRad >= 0 && angleRad <= M_PI)
        angleRad = 2 * M_PI - angleRad;
    else
        throw cRuntimeError("unexpected angle");

    return angleRad;
}

double myAngle2traci(double angleRad)
{
    if (angleRad > 0 && angleRad <= M_PI)
        angleRad = -angleRad;
    else if (angleRad > M_PI && angleRad <= 2 * M_PI)
        angleRad = 2 * M_PI - angleRad;
    else if (angleRad == 0)
        angleRad = 0;
    else
        throw cRuntimeError("unexpected angle");

    return angleRad;
}

double reverseTraci(double angle)
{
    // angle is in radians

    // no need to normalize because traci always returns between -180 and 180
    if (angle < 0)
        angle += M_PI;
    else if (angle >= 0)
        angle -= M_PI;
    else
        throw cRuntimeError("unexpected angle");

    return angle;
}

double rad2deg(double angleRad)
{
    return angleRad * 180.0 / M_PI;
}

double deg2rad(double angleDeg)
{
    return angleDeg * M_PI / 180.0;
}

double traci2cartesianAngle(double angleRad)
{

    double angle = angleRad;

    if (angle >= -M_PI && angle < 0)
        angle += 2 * M_PI;
    else if (angle >= 0 && angle < M_PI)
        angle = angle;
    else
        throw cRuntimeError("unexpected angle");

    return angle;
}

double utilTrunc(double number)
{
    // keep only up to third decimal
    return std::trunc(number * 1000.0) / 1000.0;
}

/*
 * same as FWMath::close(), except EPSILON
 */
bool close(double one, double two)
{
    return fabs(one - two) < 0.0000001;
}

double getOokBer(double snr)
{
    // Modelling OOK based on BPSK of NistErrorRate;
    // Assuming Q-func(sqrt(snr))
    double z = std::sqrt(snr / 2.0);
    double ber = 0.5 * erfc(z);
    return ber;

    // If BER is zero we can receive, else not.
    //    if (ber == 0.0)
    //        return 1.0;
    //    else
    //        return 0.0;
}

double getOokPdr(double snr, int packetLength)
{
    double ber = getOokBer(snr);

    if (ber == 0.0) {
        return 1.0;
    }

    return std::pow(1 - ber, (double) packetLength);
}

} // namespace veins
