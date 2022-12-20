#ifndef AMBULANCE_ENVP_DRLSNENCODER_H
#define AMBULANCE_ENVP_DRLSNENCODER_H

#include "ObservationEncoder.h"

/// Encoder for the approach from A deep reinforcement learning-enabled dynamic redeployment system for mobile ambulances
class DRLSNEncoder : public ObservationEncoder {
public:
    DRLSNEncoder(string path_to_data);

    variant<vector<int>, std::map<std::string, vector<int>>>
    shape(const AmbulanceEnvironmentState &status) const override;

    variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
    encode(const AmbulanceEnvironmentState &status, const DistanceCalculator &dist) const override;

    int type() const override;
private:
    /// historical average demand per our around demand locations. format id, hour of day
    float** demand_forcast;
    /// the k for factor 4: travel time of the top k occupied ambulances. Paper seems to set this value to 1.
    const int k = 1;//todo do not hardcode;
    /// The number of time periods considered for factor 1. Paper seems to set this value to 1.
    const int m = 1;//todo do not hardcode
    /// Expected time spent at hospital
    const int expected_time_at_hospital = 1800;//todo do not hardcode
};


#endif //AMBULANCE_ENVP_DRLSNENCODER_H
