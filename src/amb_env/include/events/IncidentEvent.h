#ifndef AMBULANCE_ENV_INCIDENTEVENT_H
#define AMBULANCE_ENV_INCIDENTEVENT_H


#include "Event.h"

/// This class represents an incident (i.e., emergency).
class IncidentEvent : public Event{
public: explicit IncidentEvent(int eventTime, int day_of_year, int position_in_distance_matrix, int demandLocationId,
                               int month, int weekday, int year, int priority, float lon, float lat,
                               bool transport_to_hospital, int seconds_at_incident, int time_at_hospital,
                               int total_incident_time);
    const int day_of_year;
    const int position_in_distance_matrix;
    //currently demand location corresponds to base_station_id
    const int demand_location_id;
    const int month;
    /// day of the week
    const int weekday;
    const int year;
    /// Priority of the emergency (2 = non emergency, 3 = emergency). Currently not used
    const int priority ;

    const float lon;
    const float lat;

    /// Indicates that the patient was transported to the hospital
    const bool transport_to_hospital;

    const int seconds_at_incident;
    const int time_at_hospital;
    const int total_incident_time;
};


#endif //AMBULANCE_ENV_INCIDENTEVENT_H
