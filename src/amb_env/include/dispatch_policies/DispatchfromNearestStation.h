#ifndef AMBULANCE_ENV_DISPATCHFROMNEARESTSTATION_H
#define AMBULANCE_ENV_DISPATCHFROMNEARESTSTATION_H

#include "DispatchPolicy.h"
#include "../events/IncidentEvent.h"

/// Just a very simple dispatch policy that dispatches an ambulance from the station with the lowest id that has one
/// available
class DispatchfromNearestStation : public DispatchPolicy {
public:
    explicit DispatchfromNearestStation(DistanceCalculator* distanceCalculator) : DispatchPolicy(distanceCalculator) {}

    Ambulance *dispatch(const AmbulanceEnvironmentState &status, IncidentEvent *pEvent) override;

    Ambulance &dispatch2(const AmbulanceEnvironmentState &status, IncidentEvent *incidentEvent);
};


#endif //AMBULANCE_ENV_DISPATCHFROMNEARESTSTATION_H
