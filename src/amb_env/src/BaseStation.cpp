#include "BaseStation.h"

BaseStation::BaseStation(int baseStationId, std::string name, int position_in_distance_matrix, float lon,
                         float lat)
        : base_station_id(baseStationId),
          name(name),
          position_in_distance_matrix(
                                                                                                         position_in_distance_matrix),
          lon(lon), lat(lat) {
}

void BaseStation::remove_ambulance(Ambulance& ambulance) {
    number_of_idle_ambulances--;
    ambulances.remove(&ambulance);
}

void BaseStation::add_ambulance(Ambulance& ambulance) {
    ambulances.push_front(&ambulance);
    ambulance.base_station = this;
}