#include "../../include/hospital_policies/NearestHospitalPolicy.h"
#include "../../include/DistanceCalculator.h"
#include "../../include/events/IncidentEvent.h"

NearestHospitalPolicy::NearestHospitalPolicy(DistanceCalculator *distanceCalculator) : HospitalPolicy(distanceCalculator) {

}


// find nearest
Hospital& NearestHospitalPolicy::find_hospital(const IncidentEvent &incident_event, const AmbulanceEnvironmentState &status) {

    int min_dist = distanceCalculator->travel_time(&incident_event, status.hospitals[0]);
    int nearst_hospital_idx = 0;

    for (int i = 1 ; i < status.number_of_hospitals; i++) {
        Hospital *hospital = status.hospitals[i];
        int dist2 = distanceCalculator->travel_time(&incident_event, hospital);
        if (dist2 < min_dist) {
            min_dist = dist2;
            nearst_hospital_idx = i;
        }
    }
    return *status.hospitals[nearst_hospital_idx];
}

