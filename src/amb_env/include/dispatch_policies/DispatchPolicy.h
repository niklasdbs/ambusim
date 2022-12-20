#ifndef AMBULANCE_ENV_DISPATCHPOLICY_H
#define AMBULANCE_ENV_DISPATCHPOLICY_H


#include "../Ambulance.h"
#include "../AmbulanceEnvironmentState.h"
#include "../events/IncidentEvent.h"
#include "../DistanceCalculator.h"

/// Base class for policies that dispatch the ambulances to incidents
class DispatchPolicy {

public:
    explicit DispatchPolicy(DistanceCalculator* distanceCalculator) : distanceCalculator(distanceCalculator) {}

    virtual Ambulance *dispatch(const AmbulanceEnvironmentState &status, IncidentEvent *pEvent) = 0;
    virtual ~DispatchPolicy() = default;
    DistanceCalculator* distanceCalculator;
};


#endif //AMBULANCE_ENV_DISPATCHPOLICY_H
