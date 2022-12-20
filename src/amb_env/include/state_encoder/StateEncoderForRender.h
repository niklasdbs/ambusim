#ifndef EMRL_STATEENCODERFORRENDER_H
#define EMRL_STATEENCODERFORRENDER_H

#include <vector>
#include <variant>
#include <map>
#include "../AmbulanceEnvironmentState.h"
#include "../DistanceCalculator.h"

class StateEncoderForRender {
public:
    explicit StateEncoderForRender(int number_of_base_stations) {
        this->number_of_base_stations = number_of_base_stations;
        init_incidents_at_each_demand_location();
    };
    ~StateEncoderForRender();
    int number_of_base_stations;
    std::vector<int> incidents_at_each_demand_location;
    std::tuple<py::list, py::list> encodeToRender(const AmbulanceEnvironmentState &status);
    int find_demand_location(const AmbulanceEnvironmentState &status,
                                    const DistanceCalculator &distanceCalculator,
                                    const IncidentEvent* j);
    void init_incidents_at_each_demand_location();
    void reset_incidents_at_each_demand_location();
    void update_incidents_at_each_demand_location(const AmbulanceEnvironmentState &status,
                                                  const DistanceCalculator &dist_calculator,
                                                  const IncidentEvent *j);
    py::list convert_vec_to_pylist();
};


#endif //AMBULANCE_ENV_STATEENCODERFORRENDER_H
