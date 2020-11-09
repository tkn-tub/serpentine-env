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

// This VLC app is meant for use with static mobility when we want to control the 
// mobility parameters of the vehicle with static values for angle, rotation and distance etc.

package org.car2x.veinsvlc.application.simpleVlcApp;
import org.car2x.veins.base.modules.IBaseApplLayer;

simple SimpleVlcApp like IBaseApplLayer{
    parameters:
        @class(veins::SimpleVlcApp);
        bool debug = default(false);
        int headerLength = default(0bit) @unit(bit); //header length of the application
        int packetByteLength @unit(byte);
        double beaconingFrequency @unit(hertz);
        
    gates:
        input lowerLayerIn; // from mac layer
        output lowerLayerOut; // to mac layer
        input lowerControlIn;
        output lowerControlOut;
}
