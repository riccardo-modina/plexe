#ifndef RVPLUSHUMANTRAFFIC_H_
#define RVPLUSHUMANTRAFFIC_H_

#include "plexe/mobility/TraCIBaseTrafficManager.h"

namespace plexe {

class RVPlusHumanTraffic : public TraCIBaseTrafficManager {

public:
    virtual void initialize(int stage);

    RVPlusHumanTraffic()
    {
        insertPlatoonMessage = nullptr;
        //platoonInsertDistance = 0;
        //platoonInsertHeadway = 0;
        RVInsertSpeed = 0;
        RVInsertTime = SimTime(0);
        //platoonLeaderHeadway = 0;
        //platoonSize = 0;
        RVCars = 0;
        nLanes = 0;
        humanCars = 0;
        humanLanes = 0;
    }
    virtual ~RVPlusHumanTraffic();

protected:
    // this is used to start traffic generation
    cMessage* insertPlatoonMessage;

    void insertRVs();
    void insertHumans();

    virtual void handleSelfMsg(cMessage* msg);

    SimTime RVInsertTime;
    double RVInsertSpeed;
    // vehicles to be inserted
    struct Vehicle RV;
    struct Vehicle human;

    // total number of vehicles to be injected
    int RVCars;
    // vehicles per platoon
    //int platoonSize;
    // number of lanes
    int nLanes;
    // number of human vehicles
    int humanCars;
    // number of lanes for human vehicles
    int humanLanes;
    // insert distance
    //double platoonInsertDistance;
    // insert headway
    //double platoonInsertHeadway;
    // headway for leader vehicles
    //double platoonLeaderHeadway;
    // sumo vehicle type of platooning cars
    std::string RVVType;
    // sumo vehicle type of human driven cars
    std::string humanVType;

    virtual void scenarioLoaded();
};

} // namespace plexe

#endif
