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

#include "plexe/apps/SimplePlatooningApp.h"
#include <fstream>
#include <vector>

namespace plexe {

Define_Module(SimplePlatooningApp);

void SimplePlatooningApp::initialize(int stage)
{
    BaseApp::initialize(stage);
    if (stage == 2) {
        std::cout << "CI PROVO" << std::endl;
        //accumulatedResults.clear();
        std::string sumoid = positionHelper->getExternalId();
        std::cout << "SMAPP mysumoid = " << sumoid << std::endl;
        if (sumoid == "vtypeauto.0") {
            plexeTraciVehicle->setNoisyRadarModelParams(); // con param Default
            sampleMsg = new cMessage("Getting infos from the radar");
            std::cout << "scheduling cazzomsg" << std::endl;
            scheduleAt(5.0, sampleMsg);
        }
    }
}

void SimplePlatooningApp::handleSelfMsg(cMessage* msg)
{
    if (msg == sampleMsg) {
        scheduleAt(simTime() + 0.1, sampleMsg);
        std::cout << "Sampling with the radar of: " << positionHelper->getExternalId() << std::endl;
        double dist, relSp;

        resMap rm = plexeTraciVehicle->getRadarMeasurements(dist, relSp);
        std::cout << "SIZE of rm?? = " << rm.size() << std::endl;
        // Adding time value to each resMap
        for (auto& pair : rm) {
            pair.second.push_back(simTime().dbl());
        }

        //Add the simulation Time at accumulatedResults
        accumulatedResults.insert(accumulatedResults.end(), rm.begin(), rm.end());
    }
}


void SimplePlatooningApp::finish()
{
    BaseApp::finish();
    std::cout << "SIZE of ACCRES?? = " << accumulatedResults.size() << std::endl;
    std::stringstream ss;
    ss << par("outputcsv").stringValue() << "_radar" << positionHelper->getId() << ".csv";
    std::ofstream outputFile(ss.str());
    outputFile << "module,neigh,dist,de,speed,se,angle,ae,time\n";
    if (outputFile.is_open()) {
        for (const auto& pair : accumulatedResults) {
            outputFile << positionHelper->getExternalId() << "," << pair.first;
            for (const double value : pair.second) {
                outputFile << "," << value;
            }
            outputFile << std::endl; // Vai alla riga successiva per il prossimo risultato
        }

        outputFile.close();
        std::cout << "The radar measurments have been saved in the file RadarMeas.txt (RV example dir) with the order: "
                     "distance - distance with error / speed - speed with error / angle" << std::endl;
    }
}

} // namespace plexe
