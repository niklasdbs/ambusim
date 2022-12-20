#include "../../include/observation_encoders/NoneNumpyObservationEncoder.h"

std::variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
NoneNumpyObservationEncoder::encode(const AmbulanceEnvironmentState &status, const DistanceCalculator& dist) const {
    return 0;
}

std::variant<vector<int>, std::map<std::string, vector<int>>> NoneNumpyObservationEncoder::shape(const AmbulanceEnvironmentState& status) const {
    return std::vector<int>();
}

int NoneNumpyObservationEncoder::type() const {
    return 0;
}
