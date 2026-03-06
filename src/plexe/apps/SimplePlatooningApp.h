//
// Copyright (C) 2012-2025 Michele Segata <segata@ccs-labs.org>
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

#ifndef SIMPLEPLATOONINGAPP_H_
#define SIMPLEPLATOONINGAPP_H_

#include "plexe/apps/BaseApp.h"

using resMap = std::vector<std::pair<std::string, std::vector<double>>>;

namespace plexe {

class SimplePlatooningApp : public BaseApp {

public:
    SimplePlatooningApp()
    {
    }
    
    virtual void initialize(int stage) override;
    virtual void finish() override;

    int numInitStages() const override { return 3; }

protected:
    //local
    virtual void handleLowerMsg(cMessage* msg) override;

    /**
     * Handles PlatoonBeacons
     */
    virtual void onPlatoonBeacon(const PlatooningBeacon* pb);

    //radarmattia
    cMessage* sampleMsg;
    resMap accumulatedResults;
    virtual void handleSelfMsg(cMessage* msg) override;
};

} // namespace plexe

#endif /* SIMPLEPLATOONINGAPP_H_ */
