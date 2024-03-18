//
// Copyright (C) 2014-2023 Michele Segata <segata@ccs-labs.org>
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

#include "RVPlusHumanTraffic.h"

namespace plexe {

Define_Module(RVPlusHumanTraffic);

void RVPlusHumanTraffic::initialize(int stage)
{

    TraCIBaseTrafficManager::initialize(stage);

    if (stage == 0) {

        RVCars = par("RVCars");
        //RVSize = par("RVSize");
        nLanes = par("nLanes");
        humanCars = par("humanCars");
        humanLanes = par("humanLanes");
        RVInsertTime = SimTime(par("RVInsertTime").doubleValue());
        RVInsertSpeed = par("RVInsertSpeed").doubleValue();
        //RVInsertDistance = par("RVInsertDistance").doubleValue();
        //RVInsertHeadway = par("RVInsertHeadway").doubleValue();
        //RVLeaderHeadway = par("RVLeaderHeadway").doubleValue();
        RVVType = par("RVVType").stdstringValue();
        humanVType = par("humanVType").stdstringValue();
        insertPlatoonMessage = new cMessage("");
        scheduleAt(RVInsertTime, insertPlatoonMessage);
    }
}

void RVPlusHumanTraffic::scenarioLoaded()
{
    RV.id = findVehicleTypeIndex(RVVType);
    RV.lane = -1;
    RV.position = 0;
    RV.speed = RVInsertSpeed / 3.6;
    human.id = findVehicleTypeIndex(humanVType);
    human.lane = -1;
    human.position = 0;
    human.speed = RVInsertSpeed / 3.6 - 0.01;
}

void RVPlusHumanTraffic::handleSelfMsg(cMessage* msg)
{

    TraCIBaseTrafficManager::handleSelfMsg(msg);

    if (msg == insertPlatoonMessage) {
        insertRVs();
        insertHumans();
    }
}

void RVPlusHumanTraffic::insertRVs()
{

    // keep 50 m between human vehicles (random number)
        double distance = 50;
        // total number of cars per lane
        int carsPerLane = RVCars / nLanes;
        // total length for one lane
        double totalLength = carsPerLane * (4 + distance);

        // for each lane, we create an offset to have misaligned RVs
        double* laneOffset = new double[humanLanes];
        for (int l = 0; l < humanLanes; l++) laneOffset[l] = uniform(0, 20);

        double currentPos = totalLength;
        for (int i = 0; i < carsPerLane; i++) {
            for (int l = nLanes; l < humanLanes + nLanes; l++) {
                RV.position = currentPos + laneOffset[l - nLanes];
                RV.lane = l;
                addVehicleToQueue(0, RV);
            }
            currentPos -= (4 + distance);
        }

        delete[] laneOffset;
}

void RVPlusHumanTraffic::insertHumans()
{

    // keep 50 m between human vehicles (random number)
    double distance = 50;
    // total number of cars per lane
    int carsPerLane = humanCars / humanLanes;
    // total length for one lane
    double totalLength = carsPerLane * (4 + distance);

    // for each lane, we create an offset to have misaligned RVs
    double* laneOffset = new double[humanLanes];
    for (int l = 0; l < humanLanes; l++) laneOffset[l] = uniform(0, 20);

    double currentPos = totalLength;
    for (int i = 0; i < carsPerLane; i++) {
        for (int l = nLanes; l < humanLanes + nLanes; l++) {
            human.position = currentPos + laneOffset[l - nLanes];
            human.lane = l;
            addVehicleToQueue(0, human);
        }
        currentPos -= (4 + distance);
    }

    delete[] laneOffset;
}

RVPlusHumanTraffic::~RVPlusHumanTraffic()
{
    cancelAndDelete(insertPlatoonMessage);
    insertPlatoonMessage = nullptr;
}

} // namespace plexe
