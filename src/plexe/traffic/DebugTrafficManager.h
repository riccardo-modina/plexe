#pragma once

#include "plexe/mobility/TraCIBaseTrafficManager.h"
#include <string>
#include <vector>


namespace plexe {

class DebugTrafficManager : public TraCIBaseTrafficManager {

public:
    virtual void initialize(int stage) override;

    DebugTrafficManager()
    {
        insertTrafficMessage = nullptr;
        insertSpeed = 0;
        trafficInsertTime = SimTime(0);
    }
    virtual ~DebugTrafficManager();

protected:
    // this is used to start traffic generation
    cMessage* insertTrafficMessage;
    SimTime trafficInsertTime;

    double insertSpeed;
    std::string strCarPositions;
    std::vector<double> rCarPositions;
    std::vector<double> hCarPositions;

    virtual void handleSelfMsg(cMessage* msg) override;
    void parseCarPositionsAndInsert(std::string parstringpos, int lane, double minSpeed, double maxSpeed);
    void insertCar(std::string route, int lane, double speed, double desiredSpeed, double position, std::string vtype);

private:
    int vehicleId = 0;
};

} // namespace plexe