//
// Created by Kerrie on 2022/7/6.
//


#include "observation_encoders/AmbulanceDestinationEncoder.h"
#include "Utils.h"
#include <iostream>

std::variant <vector<int>, std::map<std::string, vector < int>>>

AmbulanceDestinationEncoder::shape(const AmbulanceEnvironmentState &status) const {
    return vector<int>{status.number_of_ambulances+1};
}

std::variant <py::array_t<float>, std::map<std::string, py::array_t < float>>, int, AmbulanceEnvironmentState>

AmbulanceDestinationEncoder::encode(const AmbulanceEnvironmentState &status, const DistanceCalculator &dist) const {

    py::ssize_t ambulances = status.number_of_ambulances;
    auto destination = py::array_t<float>(ambulances+1);

    auto observation_buffer = destination.mutable_unchecked<1>();

    for(int i = 0; i < ambulances+1; i++) {
        observation_buffer(i)=-1;
    }


    if (status.ambulance_to_redeploy == nullptr){
        return destination;
    }

    //destination only consider the ambulance which is idle or already redeployed and traveing to
    //the target base station.
    for(int i = 0; i < ambulances; i++) {
        if (status.ambulances[i]->status == idle) {
            observation_buffer(i)=status.ambulances[i]->base_station->base_station_id;
        }
        else if (status.ambulances[i]->status ==traveling_to_new_base_station) {
            observation_buffer(i)=status.ambulances[i]->target_base_station->base_station_id;
        }
    }

    //store the number of base station in last index
    observation_buffer(ambulances)=status.number_of_base_stations;

    return destination;
}

int AmbulanceDestinationEncoder::type() const {
    return 0;
}
