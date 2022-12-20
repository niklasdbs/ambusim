#ifndef AMBULANCE_ENV_OBSERVATIONENCODER_H
#define AMBULANCE_ENV_OBSERVATIONENCODER_H

#include <vector>
#include <variant>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <map>
//#include "AmbulanceEnvironmentState.h"
#include "../AmbulanceEnvironmentState.h"
#include "DistanceCalculator.h"

namespace py = pybind11;
/// Helper object to translate the state of the environment into agent inputs (e.g. tensors).
class ObservationEncoder {
public:
    virtual ~ObservationEncoder() = default;

    /// Returns the shape (i.e., dimension sizes of the tensor).
    virtual std::variant<vector<int>, std::map<std::string, vector<int>>> shape(const AmbulanceEnvironmentState& status) const = 0; //todo add discrete shape (int)

    /// Create an encoding of the state
    virtual std::variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
    encode(const AmbulanceEnvironmentState& status, const DistanceCalculator& dist) const = 0;

    /// Return the type of this encoder.
    virtual int type() const = 0; //todo remove infer type from shape
};


#endif //AMBULANCE_ENV_OBSERVATIONENCODER_H