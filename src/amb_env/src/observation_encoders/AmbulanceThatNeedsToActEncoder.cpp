#include "observation_encoders/AmbulanceThatNeedsToActEncoder.h"

std::variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
AmbulanceThatNeedsToActEncoder::encode(const AmbulanceEnvironmentState &status, const DistanceCalculator& dist) const {
    if (status.ambulance_to_redeploy == nullptr){
        return -1;
    }

    return status.ambulance_to_redeploy->ambulance_id;
}

std::variant<vector<int>, std::map<std::string, vector<int>>> AmbulanceThatNeedsToActEncoder::shape(const AmbulanceEnvironmentState& status) const {
    return std::vector<int>{1};
}

int AmbulanceThatNeedsToActEncoder::type() const {
    return 0;
}
