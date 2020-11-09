//
// Copyright (C) 2018 Agon Memedi <memedi@ccs-labs.org>
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

#include "veins-vlc/analogueModel/VehicleObstacleShadowingForVlc.h"

using namespace veins;

VehicleObstacleShadowingForVlc::VehicleObstacleShadowingForVlc(cComponent* owner, VehicleObstacleControl& vehicleObstacleControl, bool useTorus, const Coord& playgroundSize)
    : VehicleObstacleShadowing(owner, vehicleObstacleControl, useTorus, playgroundSize)
{
    if (useTorus) throw cRuntimeError("VehicleObstacleShadowing does not work on torus-shaped playgrounds");
}

void VehicleObstacleShadowingForVlc::filterSignal(Signal* signal)
{
    auto potentialObstacles = vehicleObstacleControl.getPotentialObstacles(signal->getSenderPoa().pos, signal->getReceiverPoa().pos, *signal);

    if (potentialObstacles.size() < 1) return;

    *signal *= 0;
}
