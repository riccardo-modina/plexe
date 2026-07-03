#include "plexe/protocols/MisbeProtocol.h"

#include "veins/modules/mac/ieee80211p/Mac1609_4.h"
#include "veins/modules/messages/BaseFrame1609_4_m.h"

#include <cstring> // For strcmp
#include <string>

using namespace veins;

namespace plexe {

Define_Module(MisbeProtocol);

void MisbeProtocol::initialize(int stage)
{
    SimplePlatooningBeaconing::initialize(stage);
    if (stage == 0) {
        memset(&replayData, 0, sizeof(replayData));
        memset(&disruptiveData, 0, sizeof(disruptiveData));
    }
}

void MisbeProtocol::handleSelfMsg(cMessage* msg)
{
    if (msg == sendBeacon) {
        if (!onAttack){
            // not during an attack
            SimplePlatooningBeaconing::handleSelfMsg(msg);
            return;
        } else if (attackType == "dataReplay") {
            // attack of type replay
            sendReplayMessage(-1);
            scheduleAt(simTime() + beaconingInterval, sendBeacon);
        } else if (attackType == "disruptive") {
            // attack of type disruptive
            sendDisruptiveMessage(-1);
            scheduleAt(simTime() + beaconingInterval, sendBeacon);
        } else {
            if(attackType == "randomPos"){
                posx = intuniform(lower_bound_random, upper_bound_random);
                posy = intuniform(lower_bound_random, upper_bound_random);
            }
            if(attackType == "randomOffset"){
                offset = intuniform(lower_bound_offset, upper_bound_offset);
            }
            if(attackType == "randomSpeed"){
                spdx = intuniform(lower_bound_random_speed, upper_bound_random_speed);
                spdy = intuniform(lower_bound_random_speed, upper_bound_random_speed);
            }
            if(attackType == "randomOffsetSpeed"){
                offset = intuniform(lower_bound_offset_speed, upper_bound_offset_speed);
            }
            sendMisbehaviorMessage(-1);
            scheduleAt(simTime() + beaconingInterval, sendBeacon);
        }
    }
}

void MisbeProtocol::sendMisbehaviorMessage(int destinationAddress, enum PlexeRadioInterfaces interfaces)
{
    sendTo(createMisbehaviorBeacon(destinationAddress).release(), interfaces);
}

void MisbeProtocol::sendReplayMessage(int destinationAddress, enum PlexeRadioInterfaces interfaces)
{
    sendTo(createReplayBeacon(destinationAddress, replayData).release(), interfaces);
}

void MisbeProtocol::sendDisruptiveMessage(int destinationAddress, enum PlexeRadioInterfaces interfaces)
{
    sendTo(createReplayBeacon(destinationAddress, replayData).release(), interfaces);
}

std::unique_ptr<BaseFrame1609_4> MisbeProtocol::createReplayBeacon(int destinationAddress, VEHICLE_DATA attackData)
{
    // vehicle's data to be included in the message
    VEHICLE_DATA data;
    // get information about the vehicle via traci
    plexeTraciVehicle->getVehicleData(&data);
    // create and send beacon
    auto wsm = veins::make_unique<BaseFrame1609_4>("", BEACON_TYPE);
    wsm->setRecipientAddress(LAddress::L2BROADCAST());
    wsm->setChannelNumber(static_cast<int>(Channel::cch));
    wsm->setUserPriority(priority);

    // create platooning beacon with data about the car
    PlatooningBeacon* pkt = new PlatooningBeacon();
    pkt->setControllerAcceleration(attackData.u);
    pkt->setAcceleration(attackData.acceleration);
    pkt->setSpeed(attackData.speed);
    pkt->setVehicleId(myId); // id keep mine
    pkt->setPositionX(attackData.positionX);
    pkt->setPositionY(attackData.positionY);
    pkt->setTime(data.time); // time keep mine
    pkt->setLength(attackData.length);
    pkt->setSpeedX(attackData.speedX);
    pkt->setSpeedY(attackData.speedY);
    pkt->setAngle(attackData.angle);
    // this equal
    pkt->setKind(BEACON_TYPE);
    pkt->setByteLength(packetSize);
    pkt->setSequenceNumber(seq_n++);
    pkt->setWarning(warning);

    wsm->encapsulate(pkt);

    return wsm;
}

std::unique_ptr<BaseFrame1609_4> MisbeProtocol::createMisbehaviorBeacon(int destinationAddress)
{
    // vehicle's data to be included in the message
    VEHICLE_DATA data;
    // get information about the vehicle via traci
    plexeTraciVehicle->getVehicleData(&data);

    // create and send beacon
    auto wsm = veins::make_unique<BaseFrame1609_4>("", BEACON_TYPE);
    wsm->setRecipientAddress(LAddress::L2BROADCAST());
    wsm->setChannelNumber(static_cast<int>(Channel::cch));
    wsm->setUserPriority(priority);

    // create platooning beacon with data about the car
    PlatooningBeacon* pkt = new PlatooningBeacon();

    pkt->setControllerAcceleration(data.u);
    if(attackType == "eventualStop"){
        pkt->setAcceleration(acl);
    } else {
        pkt->setAcceleration(data.acceleration);
    }

    //pkt->setSpeed(data.speed);
    pkt->setVehicleId(myId);

    // POSITION MISBEHAVIOR
    if(attackType == "randomOffset"){
        pkt->setPositionX(data.positionX + offset);
        pkt->setPositionY(data.positionY + offset);
    } else if (attackType == "randomPos" or attackType == "constPos" or attackType == "eventualStop") {
        pkt->setPositionX(posx);
        pkt->setPositionY(posy);
    } else {
        pkt->setPositionX(data.positionX);
        pkt->setPositionY(data.positionY);
    }
    pkt->setTime(data.time);
    pkt->setLength(length);

    // SPEED MISBEHAVIOR
    if(attackType == "randomOffsetSpeed"){
        pkt->setSpeedX(data.speedX + offset);
        pkt->setSpeed(data.speed + offset);
        pkt->setSpeedY(data.speedY + offset);
    }  else if (attackType == "randomSpeed" or attackType == "constSpeed" or attackType == "eventualStop") {
        pkt->setSpeedX(spdx);
        pkt->setSpeed(spdx);
        pkt->setSpeedY(spdy);
    } else {
        pkt->setSpeedX(data.speedX);
        pkt->setSpeed(data.speed);
        pkt->setSpeedY(data.speedY);
    }

    pkt->setAngle(data.angle);
    pkt->setKind(BEACON_TYPE);
    pkt->setByteLength(packetSize);
    pkt->setSequenceNumber(seq_n++);

    pkt->setWarning(warning);

    wsm->encapsulate(pkt);

    return wsm;
}

std::unique_ptr<BaseFrame1609_4> MisbeProtocol::createBeacon(int destinationAddress)
{
    auto wsm = BaseProtocol::createBeacon(destinationAddress);

    PlatooningBeacon* pkt = check_and_cast<PlatooningBeacon*>(wsm->getEncapsulatedPacket());
    pkt->setWarning(warning);

    return wsm;
}

void MisbeProtocol::setWarning(bool misbehaviour)
{
    warning = misbehaviour;
}

void MisbeProtocol::activeAttack(std::string type)
{
    onAttack = true;
    attackType = type;
    if(type == "constPos"){
        VEHICLE_DATA data;
        plexeTraciVehicle->getVehicleData(&data);
        posx = data.positionX;
        posy = data.positionY;
    }
    if(type == "constSpeed"){
        VEHICLE_DATA data;
        plexeTraciVehicle->getVehicleData(&data);
        spdx = data.speedX;
        spdy = data.speedY;
    }
    if(type == "eventualStop"){
        VEHICLE_DATA data;
        plexeTraciVehicle->getVehicleData(&data);
        spdx = 0;
        spdy = 0;
        posx = 0;
        posy = 0;
        acl = 0;
    }
}

void MisbeProtocol::setReplayIndex(int index)
{
    replayIndex = index;
}

void MisbeProtocol::setReplayMessage(const PlatooningBeacon* pb)
{
    if (pb->getVehicleId() == replayIndex){
        replayData.acceleration = pb->getAcceleration();
        replayData.length = pb->getLength();
        replayData.positionX = pb->getPositionX();
        replayData.positionY = pb->getPositionY();
        replayData.speed = pb->getSpeed();
        replayData.u = pb->getControllerAcceleration();
        replayData.speedX = pb->getSpeedX();
        replayData.speedY = pb->getSpeedY();
        replayData.angle = pb->getAngle();
    }
}

void MisbeProtocol::setDisruptiveMessage(const PlatooningBeacon* pb)
{
    disruptiveData.acceleration = pb->getAcceleration();
    disruptiveData.length = pb->getLength();
    disruptiveData.positionX = pb->getPositionX();
    disruptiveData.positionY = pb->getPositionY();
    disruptiveData.speed = pb->getSpeed();
    disruptiveData.u = pb->getControllerAcceleration();
    disruptiveData.speedX = pb->getSpeedX();
    disruptiveData.speedY = pb->getSpeedY();
    disruptiveData.angle = pb->getAngle();
}

} // namespace plexe
