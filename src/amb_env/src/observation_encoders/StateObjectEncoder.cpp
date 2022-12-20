#include "observation_encoders/StateObjectEncoder.h"

std::variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
StateObjectEncoder::encode(const AmbulanceEnvironmentState &status, const DistanceCalculator& dist) const {
    return status;
}

std::variant<vector<int>, std::map<std::string, vector<int>>> StateObjectEncoder::shape(const AmbulanceEnvironmentState& status) const {
    return std::vector<int>();
}

int StateObjectEncoder::type() const {
    return 0;
}
