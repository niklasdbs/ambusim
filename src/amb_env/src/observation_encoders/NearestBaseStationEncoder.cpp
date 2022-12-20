#include "../../include/observation_encoders/NearestBaseStationEncoder.h"

std::variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
NearestBaseStationEncoder::encode(const AmbulanceEnvironmentState &status, const DistanceCalculator& dist) const {
    if (status.ambulance_to_redeploy == nullptr)
    {
        return -1;
    }

    auto hospital = status.ambulance_to_redeploy->current_hospital;
    if (hospital == nullptr) {
        auto incident = status.ambulance_to_redeploy->current_incident_event;
        int current_min_distance = INT32_MAX;
        int current_min_base_station_id = -1;
        for (BaseStation* station : status.base_stations)
        {
            auto distance = dist.travel_time(incident, station);
            if (distance < current_min_distance)
            {
                current_min_distance = distance;
                current_min_base_station_id = station->base_station_id;
            }
        }
        return current_min_base_station_id;
    }
    else{
        int current_min_distance = INT32_MAX;
        int current_min_base_station_id = -1;
        for (BaseStation* station : status.base_stations)
        {
            auto distance = dist.travel_time(hospital, station);
            if (distance < current_min_distance)
            {
                current_min_distance = distance;
                current_min_base_station_id = station->base_station_id;
            }
        }
            return current_min_base_station_id;
    }
}

std::variant<vector<int>, std::map<std::string, vector<int>>> NearestBaseStationEncoder::shape(const AmbulanceEnvironmentState& status) const {
    return std::vector<int>{1};
}

int NearestBaseStationEncoder::type() const {
    return 0;
}
