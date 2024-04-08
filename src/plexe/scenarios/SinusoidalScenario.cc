//
// Copyright (C) 2012-2023 Michele Segata <segata@ccs-labs.org>
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
#include <fstream>
#include <vector>
#include <utility>
#include "plexe/scenarios/SinusoidalScenario.h"
using resMapAccumulator = std::vector<std::pair<std::string, std::vector<double>>>;
resMapAccumulator accumulatedResults;

namespace plexe {

Define_Module(SinusoidalScenario);

void SinusoidalScenario::initialize(int stage)
{

    BaseScenario::initialize(stage);

    if (stage == 2) {
        // get the oscillation frequency of the leader as parameter
        leaderOscillationFrequency = par("leaderOscillationFrequency").doubleValue();
        // oscillation amplitude
        oscillationAmplitude = par("oscillationAmplitude").doubleValue() / 3.6;
        // average speed
        leaderSpeed = par("leaderSpeed").doubleValue() / 3.6;
        // number of lanes
        nLanes = par("nLanes").intValue();
        // start oscillation time
        startOscillating = SimTime(par("startOscillating").doubleValue());

        if (positionHelper->getId() < nLanes) {
            // setup oscillation message, only if i'm part of the first leaders
            changeSpeed = new cMessage("changeSpeed");
            if (simTime() > startOscillating) {
                startOscillating = simTime();
                scheduleAt(simTime(), changeSpeed);
            }
            else {
                scheduleAt(startOscillating, changeSpeed);
            }
            // set base cruising speed
            plexeTraciVehicle->setCruiseControlDesiredSpeed(leaderSpeed);
        }
        else {
            // let the follower set a higher desired speed to stay connected
            // to the leader when it is accelerating
            plexeTraciVehicle->setCruiseControlDesiredSpeed(leaderSpeed + 2 * oscillationAmplitude);
        }
        std::string sumoid = positionHelper->getExternalId();
        if (sumoid == "vtypeauto.0") {
            testMsg = new cMessage("Getting infos from the radar");
            scheduleAt(5.0, testMsg);
        }
    }
}

SinusoidalScenario::~SinusoidalScenario()
{
    cancelAndDelete(changeSpeed);
    changeSpeed = nullptr;
}

void SinusoidalScenario::handleSelfMsg(cMessage* msg)
{
    BaseScenario::handleSelfMsg(msg);
    if (msg == changeSpeed) {
        plexeTraciVehicle->setCruiseControlDesiredSpeed(leaderSpeed + oscillationAmplitude * sin(2 * M_PI * (simTime() - startOscillating).dbl() * leaderOscillationFrequency));
        scheduleAt(simTime() + SimTime(0.1), changeSpeed);
    } else if (msg == testMsg) {
        scheduleAt(simTime() + 0.1, testMsg);
        std::cout << positionHelper->getExternalId() << std::endl;
        double dist, relSp;
        std::vector<std::pair<std::string, std::vector<double>>> resMapAccumulator;
        std::vector<std::pair<std::string, std::vector<double>>> SimulationTime;
        double SimTime = omnetpp::simTime().dbl(); //Add the simulation Time at accumulatedResults
        std::vector<double> SimTimeVec;
        SimTimeVec.push_back(SimTime);
        SimulationTime.push_back(std::make_pair("Time", SimTimeVec));

        plexeTraciVehicle->setNoisyRadarModelParams(); // con param Default
        auto resMap = plexeTraciVehicle->getRadarMeasurements(dist, relSp);

        //Add the simulation Time at accumulatedResults
        accumulatedResults.insert(accumulatedResults.end(), SimulationTime.begin(), SimulationTime.end());
        accumulatedResults.insert(accumulatedResults.end(), resMap.begin(), resMap.end());
    }
}

void SinusoidalScenario::finish() {
    std::ofstream outputFile("output.txt");

    if (outputFile.is_open()) {
        for (const auto& pair : accumulatedResults) {
            outputFile << pair.first << ":";
            for (double value : pair.second) {
                outputFile << " " << value;
            }
            outputFile << std::endl; // Vai alla riga successiva per il prossimo risultato
        }

        outputFile.close();
        std::cout << "The radar measurments have been saved in the file output.txt (results dir) with the order: "
                     "distance distance with error speed speed with error and angle" << std::endl;
    }
} // namespace plexe
}