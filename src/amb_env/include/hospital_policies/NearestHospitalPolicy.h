#ifndef AMBULANCE_ENV_NEARESTHOSPITALPOLICY_H
#define AMBULANCE_ENV_NEARESTHOSPITALPOLICY_H

#include "HospitalPolicy.h"
#include "events/IncidentEvent.h"
#include "DistanceCalculator.h"

/// Just a very simple dispatch policy that choose a hospital with the closest distance from incidents site
class NearestHospitalPolicy : public HospitalPolicy {
public:
    explicit NearestHospitalPolicy(DistanceCalculator* distanceCalculator);
    Hospital& find_hospital(const IncidentEvent& incident_event, const AmbulanceEnvironmentState& status) override;
};


#endif //AMBULANCE_ENV_NEARESTHOSPITALPOLICY_H
