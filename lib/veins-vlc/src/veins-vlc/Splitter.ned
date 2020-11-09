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

// This module accepts packets from the application layer and distributes 
// it to the correct NIC based on the tagging on thhe packet coming 
// from the application layer. Recorgnizes both WSMs and VLC Messages 
// coming from the upper layers. 
//    
// For the messages coming from the lower layer NICs, it just sends the
// message to the application; it is the taks of the application to
// figure out if the message is a WSM or a VLC Message, in case of need
        
package org.car2x.veinsvlc;

simple Splitter like ISplitter {
    parameters:
        @class(veins::Splitter);
        bool debug = default(false);
        bool draw = default(false);
        bool collectStatistics = default(false);
        double drawHeadHalfAngle = default(0) @unit(deg);
        double drawTailHalfAngle = default(0) @unit(deg);
        
        @signal[totalVlcDelay](type="simtime_t");
        @statistic[totalVlcDelay](record=mean);
        
        @signal[headVlcDelay](type="simtime_t");
        @statistic[headVlcDelay](record=mean);
        
        @signal[tailVlcDelay](type="simtime_t");
        @statistic[tailVlcDelay](record=mean);
          
    gates:
        output applicationOut;  // output gate towards the application
        input applicationIn;    // input gate from the application
        
        output nicOut;  // output gate towards the 802.11p NIC
        input nicIn;    // input gate from the 802.11p NIC
        
        output nicVlcHeadOut;   // output gate towards the HeadLight
        input nicVlcHeadIn;     // input gate from the HeadLight
        
        output nicVlcTailOut;   // output gate towards the TailLight
        input nicVlcTailIn;     // input gate from the TailLight
}
