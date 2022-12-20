#ifndef AMBULANCE_ENV_AMBULANCEEVENT_H
#define AMBULANCE_ENV_AMBULANCEEVENT_H

#include <string>
#include "Event.h"
#include "IncidentEvent.h"

using namespace std;

enum AmbulanceEventType{arrive_at_base, arrive_at_incident, arrive_at_hospital, finished_at_hospital};

/// This class represent events of an ambulance
class AmbulanceEvent : public Event {
public:
    explicit AmbulanceEvent(int eventTime, int ambulanceId, AmbulanceEventType ambulanceEventType);
    /// Ambulance event type
    const AmbulanceEventType ambulance_event_type;
    /// ID of the ambulance in the event
    const int ambulance_id;
};

class AmbulanceOnSceneFinishedEvent : public Event{
public:
    explicit AmbulanceOnSceneFinishedEvent(int eventTime, int ambulanceId,
                                           const IncidentEvent* incident_event, bool travel_to_hospital);
    const class IncidentEvent* incident_event;
    const int ambulance_id;
    const bool travel_to_hospital;
};

#endif //AMBULANCE_ENV_AMBULANCEEVENT_H
