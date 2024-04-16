package org.car2x.plexe.traffic;

import org.car2x.plexe.mobility.BTraCIBaseTrafficManager;

simple DebugTrafficManager extends BTraCIBaseTrafficManager {
    parameters:
        @class(plexe::DebugTrafficManager);
        // double insertionHeadwayTime @unit("s");
        // double minInsertionSpace @unit("m") = default(6m);
        double insertionSpeed @unit("kmph") = default(90 kmph);
        string platooningVType = default("vtypeauto");
        string noCommVType = default("vtypehuman");
        string carPositions0 = default("15.0 10.0");
        string carPositions1 = default("");
        string carPositions2 = default("");
        string carPositions3 = default("");
        double insertionHeadwayTime = default(1.2);
}
