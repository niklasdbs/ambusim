//
// Created by Kerrie on 2022/7/6.
//

#ifndef AMBULANCE_ENVP_AMBULANCEDESTINATIONENCODER_H
#define AMBULANCE_ENVP_AMBULANCEDESTINATIONENCODER_H

#include "ObservationEncoder.h"

class AmbulanceDestinationEncoder : public ObservationEncoder{
public:
    variant<vector<int>, std::map<std::string, vector<int>>>
    shape(const AmbulanceEnvironmentState &status) const override;

    variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
    encode(const AmbulanceEnvironmentState &status, const DistanceCalculator &dist) const override;

    int type() const override;
};


#endif //AMBULANCE_ENVP_AMBULANCEDESTINATIONENCODER_H
