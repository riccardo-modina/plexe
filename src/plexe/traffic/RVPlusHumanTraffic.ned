package org.car2x.plexe.traffic;

import org.car2x.plexe.mobility.BTraCIBaseTrafficManager;

simple RVPlusHumanTraffic extends BTraCIBaseTrafficManager {

    parameters:
        //platoon insertion time
        double RVInsertTime @unit("s");
        //platoon insertion speed
        double RVInsertSpeed @unit("kmph");
        //how many vehicles to inject
        int RVCars;
        //platoon size
        //int platoonSize;
        //number of lanes
        int nLanes;
        //sumo vehicle type for platooning cars
        string RVVType;
        //how many human vehicles to inject
        int humanCars;
        //how many lanes to use for human cars
        int humanLanes;
        //sumo vehicle type for human cars
        string humanVType;
        //insert distance and headway. distance is computed as:
        //dist = insertHeadway * insertSpeed + insertDistance
        //double platoonInsertDistance @unit("m") = default(5m);
        //double platoonInsertHeadway @unit("s") = default(0s);
        //double platoonLeaderHeadway @unit("s") = default(1.2s);
        @class(plexe::RVPlusHumanTraffic);
}
