#include "../../include/dispatch_policies/DispatchfromNearestStation.h"
#include "../../include/DistanceCalculator.h"


Ambulance *DispatchfromNearestStation::dispatch(const AmbulanceEnvironmentState &status, IncidentEvent *incidentEvent) {

    int min_dist = INT32_MAX;
    int nearst_base_station_idx = 0;

    for (int i = 0; i < status.number_of_base_stations; i++) {
        BaseStation *base_station = status.base_stations[i];
        int dist = distanceCalculator->travel_time(base_station, incidentEvent);;
        if (base_station->number_of_idle_ambulances > 0 && dist <= min_dist) {
            min_dist = dist;
            nearst_base_station_idx = i;
        }
    }

    for (auto &ambulance: status.base_stations[nearst_base_station_idx]->ambulances) {
        if (ambulance->status == idle) {
            return ambulance;
        }
    }

    return nullptr;
    //throw "No more ambulance to dispatch!";
}
