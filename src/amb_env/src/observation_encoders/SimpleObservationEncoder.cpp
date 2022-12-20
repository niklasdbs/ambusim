#include "../../include/observation_encoders/SimpleObservationEncoder.h"
 std::variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
        SimpleObservationEncoder::encode(const AmbulanceEnvironmentState &status, const DistanceCalculator& dist) const{
    std::map<std::string, py::array_t<float>> current_state;

    //todo make something that makes more sense

    auto observation = py::array_t<float>(status.number_of_base_stations);
    auto observation_buffer = observation.request();
    auto *observation_buffer_ptr = static_cast<float *>(observation_buffer.ptr);

//    for (size_t idx = 0; idx<observation_buffer.shape[0]; idx++)
//    {
//        observation_buffer_ptr[idx] = idx * 10.0f;
//    }

    for(BaseStation* station : status.base_stations)
    {
        observation_buffer_ptr[station->base_station_id] = station->ambulances.size();
    }


    //current_state["ambulances"] =ambulances;
    return observation;
}

int SimpleObservationEncoder::type() const {
    return 0;
}

std::variant<vector<int>, std::map<std::string, vector<int>>> SimpleObservationEncoder::shape(const AmbulanceEnvironmentState& status) const {
    return std::vector<int> {status.number_of_base_stations};
}
