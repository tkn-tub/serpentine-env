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

#include "catch2/catch.hpp"
#include "veins/base/utils/Coord.h"
#include "GeometryTest.h"

using namespace veins;
/**
 * Angles in TraCI/Veins:
 * 90 South
 * 180 West
 * 270 North
 * 360 East
 *
 */
SCENARIO("GeometryTestCases", "[geometryTestCases]")
{

    auto da = new GeometryTest();

    // Testing ifOnLeftOfTx
    GIVEN("One sender and three recv, left, right and front of sender")
    {
        auto s = Coord(100, 100, 1);
        auto r0 = Coord(120, 80, 1);
        auto r1 = Coord(120, 100, 1);
        auto r2 = Coord(120, 120, 1);
        auto txHeading = Coord(1, 0, 0);
        WHEN("Calculating if on left or not")
        {
            bool c0 = da->getIfOnLeft(r0, s, txHeading);
            bool c1 = da->getIfOnLeft(r1, s, txHeading);
            bool c2 = da->getIfOnLeft(r2, s, txHeading);
            THEN("True, False, False")
            {
                REQUIRE(c0 == true);
                REQUIRE(c1 == false);
                REQUIRE(c2 == false);
            }
        }
        txHeading = Coord(-1, 0, 0);
        WHEN("Inverting txHeading")
        {
            bool c0 = da->getIfOnLeft(r0, s, txHeading);
            bool c1 = da->getIfOnLeft(r1, s, txHeading);
            bool c2 = da->getIfOnLeft(r2, s, txHeading);
            THEN("True, False, False")
            {
                REQUIRE(c0 == false);
                REQUIRE(c1 == false);
                REQUIRE(c2 == true);
            }
        }
    }

    // Testing bearing calculation
    GIVEN("Three nodes in one lane, all heading east, middle node is sender")
    {
        auto s = Coord(100, 100, 1);
        auto r0 = Coord(80, 100, 1);
        auto r1 = Coord(120, 100, 1);
        // <--r0    s->    <--r1
        WHEN("Transmitting from headlight to taillights")
        {
            auto r0Heading = Coord(-1, 0, 0);
            auto r1Heading = Coord(-1, 0, 0);
            bool inBearing0 = da->getIfInBearing(r0, s, r0Heading);
            bool inBearing1 = da->getIfInBearing(r1, s, r1Heading);
            double acosB0 = da->getAcosBearing(r0, s, r0Heading);
            double acosB1 = da->getAcosBearing(r1, s, r1Heading);
            bool neg = da->unitVecNegative(r0, s, r0Heading);
            THEN("Vehicle behind sender is in bearing")
            {
                REQUIRE(inBearing0 == false);
            }
            THEN("Vehicle in front of sender is in bearing")
            {
                REQUIRE(inBearing1 == true);
            }
        }
        // Here lays the problem, txHeading not included
        // r0-->    s->    r1-->
        WHEN("Transmitting from to headlight to headlights")
        {
            auto r0Heading = Coord(1, 0, 0);
            auto r1Heading = Coord(1, 0, 0);
            auto inBearing0 = da->getIfInBearing(r0, s, r0Heading);
            auto inBearing1 = da->getIfInBearing(r1, s, r1Heading);
            double acosB0 = da->getAcosBearing(r0, s, r0Heading);
            double acosB1 = da->getAcosBearing(r1, s, r1Heading);
            THEN("Vehicle behing sender is not in bearing")
            {
                REQUIRE(inBearing0 == false); // This returns true
            }
            THEN("Vehicle in front of sender is not in bearing")
            {
                REQUIRE(inBearing1 == false);
            }
        }
        // Still fails for r0
        // r0--^    s->    r1--^
        WHEN("Receivers are rotated by 45 degrees")
        {
            auto r0Heading = Coord(1, 1, 0) / sqrt(2);
            auto r1Heading = Coord(1, 1, 0) / sqrt(2);
            auto inBearing0 = da->getIfInBearing(r0, s, r0Heading);
            auto inBearing1 = da->getIfInBearing(r1, s, r1Heading);
            double acosB0 = da->getAcosBearing(r0, s, r0Heading);
            double acosB1 = da->getAcosBearing(r1, s, r1Heading);
            THEN("Vehicle behing sender is not in bearing")
            {
                REQUIRE(inBearing0 == false); // This returns true
            }
            THEN("Vehicle in front of sender is not in bearing")
            {
                REQUIRE(inBearing1 == false);
            }
        }
    }

    // Testing irradianceAngle
    GIVEN("Nodes tx, rx0, rx1, rx2, rx3")
    {
        auto txPos = Coord(100, 100);
        auto txHeading = Coord(1, 0, 0);
        auto rxPos = Coord(120, 100);
        WHEN("")
        {
            double cosIrradAngle = da->getCosIrradianceAngle(rxPos, txPos, txHeading);
            double irradAngle = da->getIrradianceAngle(cosIrradAngle);
            THEN("cosIrradianceAngle is ")
            {
                REQUIRE(cosIrradAngle == Approx(1).margin(1e-9));
            }
            THEN("irradianceAngle is ")
            {
                REQUIRE(irradAngle == Approx(0).margin(1e-9));
            }
        }
        WHEN("")
        {
            auto rxPos = Coord(100, 80);
            double cosIrradAngle = da->getCosIrradianceAngle(rxPos, txPos, txHeading);
            double irradAngle = da->getIrradianceAngle(cosIrradAngle);
            THEN("cosIrradianceAngle is ")
            {
                REQUIRE(cosIrradAngle == Approx(0).margin(1e-9));
            }
            THEN("irradianceAngle is ")
            {
                REQUIRE(irradAngle == Approx(90).margin(1e-9));
            }
        }
        WHEN("")
        {
            auto rxPos = Coord(80, 100);
            double cosIrradAngle = da->getCosIrradianceAngle(rxPos, txPos, txHeading);
            double irradAngle = da->getIrradianceAngle(cosIrradAngle);
            THEN("cosIrradianceAngle is ")
            {
                REQUIRE(cosIrradAngle == Approx(-1).margin(1e-9));
            }
            THEN("irradianceAngle is ")
            {
                REQUIRE(irradAngle == Approx(180).margin(1e-9));
            }
        }
        WHEN("")
        {
            auto rxPos = Coord(100, 120);
            double cosIrradAngle = da->getCosIrradianceAngle(rxPos, txPos, txHeading);
            double irradAngle = da->getIrradianceAngle(cosIrradAngle);
            THEN("cosIrradianceAngle is ")
            {
                REQUIRE(cosIrradAngle == Approx(0).margin(1e-9));
            }
            THEN("irradianceAngle is ")
            {
                REQUIRE(irradAngle == Approx(90).margin(1e-9));
            }
        }
    }

    // Testing incidenceAngle
    GIVEN("Nodes tx, rx0, rx1, rx2, rx3")
    {
        auto txPos = Coord(100, 100);
        auto rxPos = Coord(120, 100);
        WHEN("")
        {
            auto rxHeading = Coord(-1, 0, 0);
            double cosIncidAngle = da->getCosIncidenceAngle(rxPos, txPos, rxHeading);
            double incidAngle = da->getIncidenceAngle(cosIncidAngle);
            THEN("cosIncidenceAngle is ")
            {
                REQUIRE(cosIncidAngle == Approx(-1).margin(1e-9));
            }
            THEN("incidenceAngle is ")
            {
                REQUIRE(incidAngle == Approx(180).margin(1e-9));
            }
        }
        WHEN("")
        {
            auto rxHeading = Coord(-1, 1, 0);
            double cosIncidAngle = da->getCosIncidenceAngle(rxPos, txPos, rxHeading);
            double incidAngle = da->getIncidenceAngle(cosIncidAngle);
            THEN("cosIncidenceAngle is ")
            {
                REQUIRE(cosIncidAngle == Approx(-1).margin(1e-9));
            }
            THEN("incidenceAngle is ")
            {
                REQUIRE(incidAngle == Approx(180).margin(1e-9));
            }
        }
        WHEN("")
        {
            auto rxHeading = Coord(-1, -1, 0);
            double cosIncidAngle = da->getCosIncidenceAngle(rxPos, txPos, rxHeading);
            double incidAngle = da->getIncidenceAngle(cosIncidAngle);
            THEN("cosIncidenceAngle is ")
            {
                REQUIRE(cosIncidAngle == Approx(-1).margin(1e-9));
            }
            THEN("incidenceAngle is ")
            {
                REQUIRE(incidAngle == Approx(180).margin(1e-9));
            }
        }
    }

    double headlightMaxTxAngle = 45;
    double tx2RxDistance = 40;
    // Testing calculation of whether rx is in fov of tx
    GIVEN("Sender and receiver node heading east")
    {
        auto sendPos = Coord(100, 100, 0);
        auto recvPos = Coord(140, 100, 0);
        WHEN("Sender is directly facing receiver")
        {
            auto txHeading = Coord(cos(deg2rad(360)), sin(deg2rad(360)));
            bool inFov = da->getIfInFov(recvPos, sendPos, txHeading, tx2RxDistance, headlightMaxTxAngle);
            THEN("Receiver is in field-of-view")
            {
                REQUIRE(inFov == true);
            }
        }
        WHEN("Sender is rotated by -60 degrees")
        {
            auto txHeading = Coord(cos(deg2rad(300)), sin(deg2rad(300)));
            bool inFov = da->getIfInFov(recvPos, sendPos, txHeading, tx2RxDistance, headlightMaxTxAngle);
            THEN("Receiver is not in field-of-view")
            {
                REQUIRE(inFov == false);
            }
        }
        WHEN("Sender is rotated by +40 degrees")
        {
            auto txHeading = Coord(cos(deg2rad(40)), sin(deg2rad(40)));
            bool inFov = da->getIfInFov(recvPos, sendPos, txHeading, tx2RxDistance, headlightMaxTxAngle);
            THEN("Receiver is in field-of-view")
            {
                REQUIRE(inFov == true);
            }
        }
    }

    // Testing the calculation of light module and photodiode positions
    GIVEN("A sender at (100,100,1.5), distance between headlights 15m")
    {
        // Set position given to filterSignal() function.
        auto sendPosL = Coord(100, 100, 1.5);
        auto sendPosR = Coord(100, 100, 1.5);
        auto recvPos = Coord(100, 100, 1.5);
        float photodiodeGroundOffsetZ = 0.3;
        float interModuleDistance = 15;
        WHEN("Calculate positions facing north")
        {
            float txHeading = deg2rad(270);
            da->rotatePos(sendPosL, txHeading, 2.5, -interModuleDistance / 2, sendPosL.z);
            da->rotatePos(sendPosR, txHeading, 2.5, interModuleDistance / 2, sendPosR.z);
            da->rotatePos(recvPos, txHeading, -2.5, 0, photodiodeGroundOffsetZ);

            THEN("Left light position is (92.5, 97.5, 1.5)")
            {
                REQUIRE(sendPosL.x == Approx(92.5).margin(1e-9));
                REQUIRE(sendPosL.y == Approx(97.5).margin(1e-9));
                REQUIRE(sendPosL.z == Approx(1.5).margin(1e-9));
            }
            THEN("Right light position is (107.5, 97.5, 1.5)")
            {
                REQUIRE(sendPosR.x == Approx(107.5).margin(1e-9));
                REQUIRE(sendPosR.y == Approx(97.5).margin(1e-9));
                REQUIRE(sendPosR.z == Approx(1.5).margin(1e-9));
            }

            THEN("Position of rear photodiode is()")
            {
                REQUIRE(recvPos.x == Approx(100).margin(1e-9));
                REQUIRE(recvPos.y == Approx(102.5).margin(1e-9));
                REQUIRE(recvPos.z == Approx(0.3).margin(1e-9));
            }
        }
        // Facing east
        WHEN("Calculate positions facing east")
        {
            float txHeading = deg2rad(360);
            da->rotatePos(sendPosL, txHeading, 2.5, -interModuleDistance / 2, sendPosL.z);
            da->rotatePos(sendPosR, txHeading, 2.5, interModuleDistance / 2, sendPosR.z);
            da->rotatePos(recvPos, txHeading, -2.5, 0, photodiodeGroundOffsetZ);

            THEN("Left light position is (102.5, 92.5, 1.5)")
            {
                REQUIRE(sendPosL.x == Approx(102.5).margin(1e-9));
                REQUIRE(sendPosL.y == Approx(92.5).margin(1e-9));
                REQUIRE(sendPosL.z == Approx(1.5).margin(1e-9));
            }
            THEN("Right light position is (102.5, 92.5, 1.5)")
            {
                REQUIRE(sendPosR.x == Approx(102.5).margin(1e-9));
                REQUIRE(sendPosR.y == Approx(107.5).margin(1e-9));
                REQUIRE(sendPosR.z == Approx(1.5).margin(1e-9));
            }

            THEN("Position of rear photodiode is (97.5, 100, 0.3)")
            {
                REQUIRE(recvPos.x == Approx(97.5).margin(1e-9));
                REQUIRE(recvPos.y == Approx(100).margin(1e-9));
                REQUIRE(recvPos.z == Approx(0.3).margin(1e-9));
            }
        }
        // Facing south
        WHEN("Calculate positions facing south")
        {
            float txHeading = deg2rad(90);
            da->rotatePos(sendPosL, txHeading, 2.5, -interModuleDistance / 2, sendPosL.z);
            da->rotatePos(sendPosR, txHeading, 2.5, interModuleDistance / 2, sendPosR.z);
            da->rotatePos(recvPos, txHeading, -2.5, 0, photodiodeGroundOffsetZ);

            THEN("Left light position is (107.5, 102.5, 1.5)")
            {
                REQUIRE(sendPosL.x == Approx(107.5).margin(1e-9));
                REQUIRE(sendPosL.y == Approx(102.5).margin(1e-9));
                REQUIRE(sendPosL.z == Approx(1.5).margin(1e-9));
            }
            THEN("Right light position is (92.5, 102.5, 1.5)")
            {
                REQUIRE(sendPosR.x == Approx(92.5).margin(1e-9));
                REQUIRE(sendPosR.y == Approx(102.5).margin(1e-9));
                REQUIRE(sendPosR.z == Approx(1.5).margin(1e-9));
            }

            THEN("Position of rear photodiode is()")
            {
                REQUIRE(recvPos.x == Approx(100).margin(1e-9));
                REQUIRE(recvPos.y == Approx(97.5).margin(1e-9));
                REQUIRE(recvPos.z == Approx(0.3).margin(1e-9));
            }
        }
        // Facing west
        WHEN("Calculate positions facing west")
        {
            float txHeading = deg2rad(180);
            da->rotatePos(sendPosL, txHeading, 2.5, -interModuleDistance / 2, sendPosL.z);
            da->rotatePos(sendPosR, txHeading, 2.5, interModuleDistance / 2, sendPosR.z);
            da->rotatePos(recvPos, txHeading, -2.5, 0, photodiodeGroundOffsetZ);

            THEN("Left light position is (97.5, 107.5, 1.5)")
            {
                REQUIRE(sendPosL.x == Approx(97.5).margin(1e-9));
                REQUIRE(sendPosL.y == Approx(107.5).margin(1e-9));
                REQUIRE(sendPosL.z == Approx(1.5).margin(1e-9));
            }
            THEN("Right light position is (97.5, 92.5, 1.5)")
            {
                REQUIRE(sendPosR.x == Approx(97.5).margin(1e-9));
                REQUIRE(sendPosR.y == Approx(92.5).margin(1e-9));
                REQUIRE(sendPosR.z == Approx(1.5).margin(1e-9));
            }

            THEN("Position of rear photodiode is (97.5, 100, 0.3)")
            {
                REQUIRE(recvPos.x == Approx(102.5).margin(1e-9));
                REQUIRE(recvPos.y == Approx(100).margin(1e-9));
                REQUIRE(recvPos.z == Approx(0.3).margin(1e-9));
            }
        }
    }
}
