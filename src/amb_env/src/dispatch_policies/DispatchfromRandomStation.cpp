#include "../../include/dispatch_policies/DispatchfromNearestStation.h"
//#include "../distance_calculator/DistanceCalculator.h"

#include "../../include/dispatch_policies/DispatchfromRandomStation.h"

// choose random base station
Ambulance *DispatchfromRandomStation::dispatch(const AmbulanceEnvironmentState &status, IncidentEvent *incidentEvent) {

    int min = 0;
    int max = status.number_of_base_stations-1;
    while(true) {
        int idx = ((int) rand() / (RAND_MAX+1)) * (max-min+1) + min;
        BaseStation *base_station = status.base_stations[idx];
        if (base_station->number_of_idle_ambulances > 0) {
            for (auto &ambulance: base_station->ambulances) {
                if (ambulance->status == idle) {
                    return ambulance;
                }
            }
        }
    }

    return nullptr;
}

