#ifndef AMBULANCE_ENV_DISPATCHFROMRANDOMSTATION_H
#define AMBULANCE_ENV_DISPATCHFROMRANDOMSTATION_H

#include "DispatchPolicy.h"
#include "../events/IncidentEvent.h"

class DispatchfromRandomStation : public DispatchPolicy {
public:
    Ambulance *dispatch(const AmbulanceEnvironmentState &status, IncidentEvent *pEvent) override;
};


#endif //AMBULANCE_ENV_DISPATCHFROMRANDOMSTATION_H