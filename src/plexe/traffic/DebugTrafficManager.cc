#include "DebugTrafficManager.h"
#include <string>

namespace plexe {

Define_Module(DebugTrafficManager);

void DebugTrafficManager::initialize(int stage)
{

    TraCIBaseTrafficManager::initialize(stage);

    if (stage == 0) {
        insertSpeed = par("insertionSpeed").doubleValueInUnit("mps");

        insertTrafficMessage = new cMessage("insert cars");
        scheduleAt(1.0, insertTrafficMessage);
    }
}

void DebugTrafficManager::insertCar(std::string route, int lane, double speed,
    double desiredSpeed, double position, std::string vtype)
{
    Vehicle traci_info = {
        .id = findVehicleTypeIndex(vtype),
        .lane = lane,
        .position = (float) position,
    };
    traci_info.speed = (float) speed;
    traci_info.ccDesiredSpeed = desiredSpeed;
    addVehicleToQueue(route, traci_info);
    // if human veh, no need to configure platooning Info
    if (vtype == par("humanVType").stringValue()) return;

    if (vtype != par("radarVType").stringValue())
        throw cRuntimeError("If not human this traffci manager should only create radarVtype vehsicles");

    // pupoulate also Plexe info for this veh
    VehicleInfo vehicle_info = {
        .controller = ACC, // initially vehs are all ACC
        .distance = 5, // if ever used by a PATH veh...
        .headway = 1.2,
        .id = this->vehicleId,
        .platoonId = this->vehicleId++,
        .position = 0,
    };
    PlatoonInfo platoon_info{
        .speed = traci_info.speed,
        .lane = traci_info.lane};

    positions.addVehicleToPlatoon(vehicle_info.id, vehicle_info);
    positions.setPlatoonInformation(vehicle_info.platoonId, platoon_info);
}

void DebugTrafficManager::parseCarPositionsAndInsert(std::string parstringpos, int lane, double minSpeed, double maxSpeed)
{
    strCarPositions = parstringpos;
    //std::cout << "strCarPositions = " << strCarPositions << "\n";
    std::vector<std::string> vecs = cStringTokenizer(strCarPositions.c_str()).asVector();
    rCarPositions.clear();
    hCarPositions.clear();
    for (auto sp : vecs) {
        std::vector<std::string> carp = cStringTokenizer(sp.c_str(), "-").asVector();
        if (carp[0] == "h") {
            std::cout << "adding humanVeh at pos: " << carp[1] << std::endl;
            this->hCarPositions.push_back(std::stod(carp[1]));
        }
        else if (carp[0] == "r") {
            std::cout << "adding radarVeh at pos: " << carp[1] << std::endl;
            this->rCarPositions.push_back(std::stod(carp[1]));
        }
    }
    std::string radarVType = par("radarVType").stringValue();
    std::string humanVType = par("humanVType").stringValue();
    for (auto carpos : rCarPositions)
        insertCar("platoon_route", lane, insertSpeed, uniform(minSpeed, maxSpeed), carpos, radarVType);
    for (auto carpos : hCarPositions)
        insertCar("platoon_route", lane, insertSpeed, uniform(minSpeed, maxSpeed), carpos, humanVType);
}

void DebugTrafficManager::handleSelfMsg(cMessage* msg)
{
    std::string s;
    double minSpeed = 90 / 3.6;
    double maxSpeed = 130 / 3.6;
    int lane = 0;
    if (msg == insertTrafficMessage) {
        s = par("carPositions0").stringValue();
        if (!s.empty()) {
            lane = 0;
            parseCarPositionsAndInsert(s, lane, minSpeed, maxSpeed);
        }
        s = par("carPositions1").stringValue();
        if (!s.empty()) {
            lane = 1;
            parseCarPositionsAndInsert(s, lane, minSpeed, maxSpeed);
        }
        s = par("carPositions2").stringValue();
        if (!s.empty()) {
            lane = 2;
            parseCarPositionsAndInsert(s, lane, minSpeed, maxSpeed);
        }
        s = par("carPositions3").stringValue();
        if (!s.empty()) {
            lane = 3;
            parseCarPositionsAndInsert(s, lane, minSpeed, maxSpeed);
        }
    }
}

DebugTrafficManager::~DebugTrafficManager()
{
    cancelAndDelete(insertTrafficMessage);
    insertTrafficMessage = nullptr;
}

} // namespace plexe