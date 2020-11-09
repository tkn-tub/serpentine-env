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

#pragma once

#include "veins/base/phyLayer/AnalogueModel.h"
#include "veins/base/modules/BaseWorldUtility.h"
#include "veins/modules/analogueModel/VehicleObstacleShadowing.h"
#include "veins/modules/obstacle/VehicleObstacleControl.h"
#include "veins/base/utils/Move.h"
#include "veins/base/messages/AirFrame_m.h"

#include <cstdlib>

namespace veins {

/**
 * @brief Basic implementation of a VehicleObstacleShadowingForVlc
 * which subclasses VehicleObstacleShadowing
 *
 * @ingroup analogueModels
 */
class VehicleObstacleShadowingForVlc : public VehicleObstacleShadowing {

public:
    /**
     * @brief Initializes the analogue model. myMove and playgroundSize
     * need to be valid as long as this instance exists.
     *
     * The constructor needs some specific knowledge in order to create
     * its mapping properly:
     *
     * @param vehicleObstacleControl reference to global VehicleObstacleControl module
     * @param useTorus information about the playground the host is moving in
     * @param playgroundSize information about the playground the host is moving in
     */
    VehicleObstacleShadowingForVlc(cComponent* owner, VehicleObstacleControl& vehicleObstacleControl, bool useTorus, const Coord& playgroundSize);

    /**
     * @brief Filters a specified Signal by adding an attenuation
     * over time to the Signal.
     */
    virtual void filterSignal(Signal* signal) override;

    virtual bool neverIncreasesPower() override
    {
        return true;
    }
};

} // namespace veins
