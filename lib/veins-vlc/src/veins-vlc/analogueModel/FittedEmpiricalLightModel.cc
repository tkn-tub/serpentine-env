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

#include "veins-vlc/analogueModel/FittedEmpiricalLightModel.h"

using namespace veins;

namespace veins {

double getPowerDistance_dbm(double distance, double alpha, double beta, double gamma)
{
    return alpha + 10 * beta * log10(1 / (distance + gamma));
}

double getPowerAngle_dbm(double angle, double period, double delta, double epsilon)
{
    return delta + epsilon * cos(2 * M_PI * angle / period);
}

double getTotalPower_dbm(double distance, double angle, double alpha, double beta, double gamma, double period, double delta, double epsilon)
{
    return getPowerDistance_dbm(distance, alpha, beta, gamma) + getPowerAngle_dbm(angle, period, delta, epsilon);
}

double getTotalPowerCoord_dbm(Coord senderPos, Coord receiverPos, double alpha, double beta, double gamma, double period, double delta, double epsilon)
{
    double distance = senderPos.distance(receiverPos);
    double angle = (atan2(receiverPos.x - senderPos.x, receiverPos.y - senderPos.y) * 180 / M_PI) + 90;
    // Here from the angle we cannot tell on which side of the Tx, the Rx is located, don't use this function!
    return getTotalPower_dbm(distance, angle, alpha, beta, gamma, period, delta, epsilon);
}

} // namespace veins
