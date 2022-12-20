#ifndef AMBULANCE_ENV_BASESTATION_H
#define AMBULANCE_ENV_BASESTATION_H


#include <list>
#include "Ambulance.h"
#include <string>

class Ambulance;

/// This class represents a base station
class BaseStation {
public:
    explicit BaseStation(int baseStationId, std::string name, int position_in_distance_matrix, float lon,
                         float lat);
    /// ID of the base station  (should correspond to the position of the base station in the array)
    const int base_station_id;

    const float lon;
    const float lat;

    const std::string name;

    const int position_in_distance_matrix;

    /// Number of idle ambulance at the base station
    int number_of_idle_ambulances = 0;

    /// List that contains the ambulances located at this base station (and ambulances travelling to the station)
    std::list<Ambulance*> ambulances;

    /// Assigns an ambulance to this base station
    void add_ambulance(Ambulance& ambulance);

    /// Removes an ambulance from the base station (this method should only be called from ambulance.remove_from_base_station())
    void remove_ambulance(Ambulance& ambulance);
};


#endif //AMBULANCE_ENV_BASESTATION_H
