#include <cstdlib>
#include "AmbulanceEnvironmentState.h"

AmbulanceEnvironmentState::AmbulanceEnvironmentState(int number_of_ambulances, int number_of_base_stations, int number_of_hospitals) : number_of_ambulances(number_of_ambulances), number_of_base_stations(number_of_base_stations), number_of_hospitals(number_of_hospitals){
    ambulances = std::vector<Ambulance*>(number_of_ambulances);
    base_stations = std::vector<BaseStation*>(number_of_base_stations);
    hospitals = std::vector<Hospital*>(number_of_hospitals);
}


AmbulanceEnvironmentState::~AmbulanceEnvironmentState() = default;