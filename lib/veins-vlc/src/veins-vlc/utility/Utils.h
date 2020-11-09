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

#include <cmath>

#include <omnetpp.h>

namespace veins {

using namespace omnetpp;

/*
 * My angle representation
 * M_PI*1/2 = 90     South
 * M_PI*1 = 180      West
 * M_PI*3/2 = 270    North
 * M_PI*2 = 360      East
 */

/*
 * Transform the heading of the vehicle from traci
 * to my angle representation
 */
double traci2myAngle(double angleRad);

/*
 * Transform the heading of the vehicle from my
 * angle representation to traci
 */

double myAngle2traci(double angleRad);

// Reverse the heading of the vehicle
double reverseTraci(double angleRad);

double rad2deg(double angleRad);

double deg2rad(double angleDeg);

double traci2cartesianAngle(double angleRad);

double utilTrunc(double number);

// Based on close() from FWMath.h
bool close(double one, double two);

// Return BER of OOK at the given SNR.
double getOokBer(double snr);

double getOokPdr(double snr, int packetLength);

} // namespace veins
