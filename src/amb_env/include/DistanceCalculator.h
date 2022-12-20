#ifndef AMBULANCE_ENV_DISTANCECALCULATOR_H
#define AMBULANCE_ENV_DISTANCECALCULATOR_H

#include "BaseStation.h"
#include "events/IncidentEvent.h"

class DistanceCalculator {
public:
    explicit DistanceCalculator(double** station_to_incident, double** incident_to_station, double** incident_to_hospital, double** hospital_to_station, double** station_to_station);

    double** station_to_incident;
    double** incident_to_hospital;
    double** hospital_to_station;
    double** incident_to_station;
    double** station_to_station;

    int travel_time(const BaseStation* i, const IncidentEvent* j) const;
    int travel_time(const IncidentEvent* i, const BaseStation* j) const;
    int travel_time(const IncidentEvent* i, const Hospital* j) const;
    int travel_time(const Hospital* i, const BaseStation* j) const;
    int travel_time(const BaseStation* i, const BaseStation* j) const;
};


#endif //AMBULANCE_ENV_DISTANCECALCULATOR_H
