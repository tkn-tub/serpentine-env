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
#include <iostream>
#include <fstream>
#include <algorithm>

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "veins/modules/utility/TimerManager.h"
#include "veins/modules/world/annotations/AnnotationManager.h"
#include "veins-vlc/utility/Utils.h"

#include "veins/modules/messages/BaseFrame1609_4_m.h"
#include "veins-vlc/messages/VlcMessage_m.h"

namespace veins {

class SimpleVlcApp : public cSimpleModule {
protected:
    bool debug;

    int toLower;
    int fromLower;
    int byteLength;
    double transmissionPeriod;

    std::string sumoId;

    veins::TimerManager timerManager{this};
    mutable TraCIScenarioManager* traciManager;
    TraCIMobility* mobility;
    AnnotationManager* annotations;
    TraCICommandInterface* traci;

public:
    SimpleVlcApp();
    ~SimpleVlcApp();
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage* msg);
    virtual int numInitStages() const
    {
        return 4;
    }

protected:
    VlcMessage* generateVlcMessage(int accessTechnology);
};

} // namespace veins
