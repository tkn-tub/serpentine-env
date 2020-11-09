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

// Based on ConnectionManager.ned from Steffen Sroka, Daniel Willkomm, Karl Wessel, Michael Swigulski

package org.car2x.veinsvlc;

simple VlcConnectionManager
{
    parameters:
        @class(veins::VlcConnectionManager);
        // debug switch for core framework
        bool coreDebug = default(false);
        // send directly to the node or create separate gates for every connection
        bool sendDirect = default(true);
        // minimum carrier frequency of the channel [Hz]
//        double carrierFrequency @unit(Hz);
        // should the maximum interference distance be displayed for each node?
        bool drawMaxIntfDist = default(false);
        
        @display("i=abstract/multicast");
}

