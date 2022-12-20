#include <math.h>

#include "../include/DistanceCalculator.h"


 int DistanceCalculator::travel_time(const BaseStation* i, const IncidentEvent* j) const {
    return int(station_to_incident[i->position_in_distance_matrix][j->position_in_distance_matrix]);
}

int DistanceCalculator::travel_time(const IncidentEvent* i, const BaseStation* j) const {
    return int(incident_to_station[i->position_in_distance_matrix][j->position_in_distance_matrix]);
}


int DistanceCalculator::travel_time(const IncidentEvent* i, const Hospital* j) const {
    return int(incident_to_hospital[i->position_in_distance_matrix][j->position_in_distance_matrix]);
}

int DistanceCalculator::travel_time(const Hospital* i, const BaseStation* j) const {
    return int(hospital_to_station[i->position_in_distance_matrix][j->position_in_distance_matrix]);
}

int DistanceCalculator::travel_time(const BaseStation* i, const BaseStation* j) const {
    return int(station_to_station[i->position_in_distance_matrix][j->position_in_distance_matrix]);
}


DistanceCalculator::DistanceCalculator(double **station_to_incident,
                                       double **incident_to_station,
                                       double **incident_to_hospital,
                                       double **hospital_to_station,
                                       double **station_to_station) :
                                       station_to_incident(station_to_incident),
                                       incident_to_station(incident_to_station),
                                       incident_to_hospital(incident_to_hospital),
                                       hospital_to_station(hospital_to_station),
                                       station_to_station(station_to_station){

}

