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

#include <omnetpp.h>
#include <math.h>
#include "veins/base/utils/Coord.h"

namespace veins {

/**
 * @brief This class calculates the received power based on
 * its distance and angle from the Tx. The formulas are
 * derived via curve-fitting the measurements of the @class EmpiricalLightModel.
 * The FittedEmpiricalLightModel is used to infer the received
 * power in the distance which have not been measured for the EmpiricalLightModel.
 *
 * Avoid using this model for angles outside of the field-of-view
 *
 * @ingroup analogueModels
 *
 * @author Agon Memedi
 */

double getPowerDistance_dbm(double distance, double alpha, double beta, double gamma);

double getPowerAngle_dbm(double angle, double period, double delta, double epsilon);

double getTotalPower_dbm(double distance, double angle, double alpha, double beta, double gamma, double period, double delta, double epsilon);

double getTotalPowerCoord_dbm(Coord senderPos, Coord receiverPos, double alpha, double beta, double gamma, double period, double delta, double epsilon);

} // namespace veins
