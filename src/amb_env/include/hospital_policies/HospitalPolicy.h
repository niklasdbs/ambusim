#ifndef AMBULANCE_ENV_HOSPITALPOLICY_H
#define AMBULANCE_ENV_HOSPITALPOLICY_H

#include "Hospital.h"
#include "events/IncidentEvent.h"
#include "AmbulanceEnvironmentState.h"
#include "DistanceCalculator.h"

/// Base class for policies that find a hospital with less traveling time
class HospitalPolicy {
public:
    explicit HospitalPolicy(DistanceCalculator *distanceCalculator) :distanceCalculator(distanceCalculator) {}

    virtual Hospital& find_hospital(const IncidentEvent& incident_event, const AmbulanceEnvironmentState& status) = 0;
    virtual ~HospitalPolicy() = default;
    DistanceCalculator* distanceCalculator;
};


#endif //AMBULANCE_ENV_HOSPITALPOLICY_H
