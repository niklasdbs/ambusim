#include "events/IncidentEvent.h"

IncidentEvent::IncidentEvent(int eventTime, int day_of_year, int position_in_distance_matrix, int demandLocationId,
                             int month, int weekday, int year, int priority, float lon, float lat,
                             bool transport_to_hospital, int seconds_at_incident, int time_at_hospital,
                             int total_incident_time) : Event(eventTime, incident_event),
                                                        day_of_year(day_of_year),
                                                        position_in_distance_matrix(position_in_distance_matrix),
                                                        demand_location_id(demandLocationId),
                                                        month(month),
                                                        weekday(weekday),
                                                        year(year), priority(priority), lon(lon),
                                                        lat(lat),
                                                        transport_to_hospital(
                                                                                                                         transport_to_hospital),
                                                        seconds_at_incident(seconds_at_incident),
                                                        time_at_hospital(
                                                                                                                  time_at_hospital),
                                                        total_incident_time(total_incident_time) {

}
