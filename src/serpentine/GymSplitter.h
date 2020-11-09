//
// Copyright (C) 2019-2020 Dominik S. Buse <buse@ccs-labs.org>, Max Schettler <schettler@ccs-labs.org>
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

#include "veins-vlc/Splitter.h"
#include "serpentine/GymConnection.h"

#include <array>
#include <memory>

class GymSplitter : public veins::Splitter {
public:
    GymSplitter() = default;
    virtual ~GymSplitter();
    void initialize() override;
    void handleLowerMessage(cMessage* msg) override;
    long getLastReceivedId() const { return lastReceivedId; }

protected:
    Interfaces getAccessTechnology(cPacket *msg) override;
    GymConnection *gymCon = nullptr;

private:
    double success_reward;
    double dsrc_cost;
    double vlc_cost;
    double max_range;

    double computeReward(const GymSplitter* leaderSplitter) const;
    std::array<double, 4> computeObservation(const TraCIMobility* leaderMobility) const;
    veinsgym::proto::Request serializeObservation(const std::array<double, 4> &observation, double reward) const;

    bool isFollower = false;
    // Problem: Erste transmission
    long lastSentId = -1;
    long lastReceivedId = -2;
    Interfaces lastChoice = {};
};
