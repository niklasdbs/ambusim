#ifndef AMBULANCE_ENV_SIMPLEOBSERVATIONENCODER_H
#define AMBULANCE_ENV_SIMPLEOBSERVATIONENCODER_H

#include "ObservationEncoder.h"

using namespace std;

class SimpleObservationEncoder : public ObservationEncoder {
public:

    /// Returns the shape (i.e., dimension sizes of the tensor).
    std::variant<vector<int>, std::map<std::string, vector<int>>> shape(const AmbulanceEnvironmentState& status) const override;

    /// Create an encoding of the state
    variant<py::array_t<float>, map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
    encode(const AmbulanceEnvironmentState& status, const DistanceCalculator& dist) const override;

    /// Return the type of this encoder. (todo define type)
    int type() const override;
};


#endif //AMBULANCE_ENV_SIMPLEOBSERVATIONENCODER_H
