//
// Copyright (C) 2019 Agon Memedi <memedi@ccs-labs.org>
//
// Documentation for these modules is at http://veins.car2x.org/
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


import org.car2x.veins.base.connectionManager.ConnectionManager;
import org.car2x.veins.base.modules.BaseWorldUtility;
import org.car2x.veins.modules.mobility.traci.TraCIScenarioManager*;
import org.car2x.veins.modules.obstacle.ObstacleControl;
import org.car2x.veins.modules.obstacle.VehicleObstacleControl;
import org.car2x.veinsvlc.CarVlcMobility;
import org.car2x.veinsvlc.VlcConnectionManager;
import org.car2x.veins.modules.world.annotations.AnnotationManager;


module Scenario
{
    parameters:
        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)
        int nodesCount = default(0);
        @display("bgb=855,633");

    submodules:
//        moduleType[maxNum] -- The max number of the modules of type moduleType

 		// Total number of vehicles should be 7
        node[nodesCount]: CarVlcMobility {
            @display("p=349,206");
        }
        obstacles: ObstacleControl {
            @display("p=390,35");
        }
        vehicleObstacles: VehicleObstacleControl {
            @display("p=261,35");
        }
        annotations: AnnotationManager {
            @display("p=485,35");
        }
        connectionManager: ConnectionManager {
            parameters:
                @display("p=146,31;i=abstract/multicast");
        }
        vlcConnectionManager: VlcConnectionManager {
            parameters:
                @display("p=146,112;i=abstract/multicast");
        }
        world: BaseWorldUtility {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                playgroundSizeZ = playgroundSizeZ;
                @display("p=34,31;i=misc/globe");
        }
        manager: TraCIScenarioManagerLaunchd {
            parameters:
                @display("p=34,206");
        }

    connections allowunconnected:
}
