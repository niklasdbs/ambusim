#include "events/AmbulanceEvent.h"

AmbulanceEvent::AmbulanceEvent(int eventTime, const int ambulanceId, AmbulanceEventType ambulanceEventType)
        : Event(eventTime, ambulance_event), ambulance_id(ambulanceId), ambulance_event_type(ambulanceEventType) {

}


AmbulanceOnSceneFinishedEvent::AmbulanceOnSceneFinishedEvent(int eventTime, int ambulanceId,
                                                             const IncidentEvent* incident_event, bool travel_to_hospital)
              : Event(eventTime, ambulance_on_scene_finished_event), ambulance_id(ambulanceId), incident_event(incident_event),
                travel_to_hospital(travel_to_hospital){

}
