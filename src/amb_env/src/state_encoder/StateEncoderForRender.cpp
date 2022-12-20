//Pyind11 connect
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include "../../include/state_encoder/StateEncoderForRender.h"


std::tuple<py::list, py::list> StateEncoderForRender::encodeToRender(const AmbulanceEnvironmentState &status) {
    py::list idle_ambs_at_each_base_station;

    for (int i = 0; i < status.number_of_base_stations ; i++) {
        int num_idle_ambs = status.base_stations[i]->number_of_idle_ambulances;
        idle_ambs_at_each_base_station.append(num_idle_ambs);
    }
    return {idle_ambs_at_each_base_station, convert_vec_to_pylist()};
}

int StateEncoderForRender::find_demand_location(const AmbulanceEnvironmentState &status, const DistanceCalculator &distanceCalculator, const IncidentEvent* j) {
    int dist = INT32_MAX;
    int demand_location_idx;
    for(int i = 0; i < status.number_of_base_stations; i++) {
        int next_dist = distanceCalculator.station_to_incident[i][j->position_in_distance_matrix];
        if (next_dist < dist) {
            dist = next_dist;
            demand_location_idx = i;
        }
    }
    return demand_location_idx;
}

void StateEncoderForRender::init_incidents_at_each_demand_location() {
    for (int i = 0; i < number_of_base_stations; i++) {
        incidents_at_each_demand_location.push_back(0);
    }
}

void StateEncoderForRender::reset_incidents_at_each_demand_location() {
    for (int i = 0; i < number_of_base_stations; i++) {
        incidents_at_each_demand_location.at(i) = 0;
    }
}

void StateEncoderForRender::update_incidents_at_each_demand_location(const AmbulanceEnvironmentState &status, const DistanceCalculator &dist_calculator, const IncidentEvent* j) {
    int to_update_idx = find_demand_location(status, dist_calculator, j);
    incidents_at_each_demand_location[to_update_idx] += 1;
}

py::list StateEncoderForRender::convert_vec_to_pylist() {
    py::list pylist;
    for(int i = 0; i < incidents_at_each_demand_location.size(); i++) {
        pylist.append(incidents_at_each_demand_location.at(i));
    }
    return pylist;
}

StateEncoderForRender::~StateEncoderForRender() = default;



