#include "observation_encoders/DRLSNEncoder.h"
#include "Utils.h"

std::variant <vector<int>, std::map<std::string, vector < int>>>

DRLSNEncoder::shape(const AmbulanceEnvironmentState &status) const {
    return vector<int>{status.number_of_base_stations, (m + 2 + k)};
}

std::variant <py::array_t<float>, std::map<std::string, py::array_t < float>>, int, AmbulanceEnvironmentState>

DRLSNEncoder::encode(const AmbulanceEnvironmentState &status, const DistanceCalculator &dist) const {
    //factors for each station
    //factor 1: expected number of nearby ems requests near the base station (nearest station) in the next hour and hour after (use historical average)
    //dim: 2

    //factor 2: number of ambulances that each station contains from perspective of EMS system. This includes also non idle ones heading to there.
    //dim: 1

    //factor 3: travel time for current ambulance to reach station
    //dim: 1

    //factor 4: travel time from ambulances that are transporting patients to hospitals to station. Other ambulances are filled with two hours. Select smallest k
    //dim k

    //total dim: (m+2+k)*#stations

    //buffer used to get top k ambulances
    vector<float> times_factor_4 = vector<float>(status.number_of_ambulances);
    const int observation_size_per_station = (m+2+k);
    auto observation = py::array_t<float, py::array::c_style>({status.number_of_base_stations, observation_size_per_station});

    auto observation_buffer = observation.mutable_unchecked<2>();
    int current_hour = (status.current_time % (24*60*60)) / (60*60);
    int start_of_hour = (status.current_time / (60*60)) * 60 * 60;
    float alpha = static_cast<float>(status.current_time - start_of_hour)/ 3600.0f;

    for (BaseStation* station: status.base_stations)
    {
        int i = station->base_station_id; //station index

        //factor 1
        for(int t=0; t< m; t++){
            int t_current = (current_hour + t) % 24;
            int t_next = (current_hour + t + 1) % 24;

            float j_lambda = demand_forcast[station->base_station_id][t_current]
                             + (alpha *
                             (demand_forcast[station->base_station_id][t_next]
                             - demand_forcast[station->base_station_id][t_current] ));


            observation_buffer(i, t) = j_lambda;
        }


        //factor 2
        observation_buffer(i, m) = static_cast<float>(station->ambulances.size());

        //factor 3
        if (status.ambulance_to_redeploy != nullptr && status.ambulance_to_redeploy->status == at_hospital) {
            observation_buffer(i, m+1) = static_cast<float>(dist.travel_time(
                    status.ambulance_to_redeploy->current_hospital, station)) / 3600.0f;
        }
        else if (status.ambulance_to_redeploy != nullptr && status.ambulance_to_redeploy->status == at_incident){
            observation_buffer(i, m+1) = static_cast<float>(dist.travel_time(
                    status.ambulance_to_redeploy->current_incident_event, station)) / 3600.0f;
        }
        else{
            observation_buffer(i, m+1) = 0.0;
        }
        //factor 4
        for (Ambulance* ambulance : status.ambulances)
        {
            float travel_time;

            if (ambulance->status == traveling_to_hospital)
            {
                //current time - arrival time at hospital => need to store arrival time at hospital in ambulance
                travel_time += static_cast<float>(status.current_time-ambulance->expected_arrival_time_at_hospital);
                travel_time += static_cast<float>(expected_time_at_hospital);//time at hospital
                travel_time += static_cast<float>(dist.travel_time(ambulance->current_hospital, station));
            }
            else{
                travel_time = 2 * 60 * 60;//two hours
            }

            times_factor_4[ambulance->ambulance_id] = travel_time / 3600.0f;
        }

        std::sort(times_factor_4.begin(), times_factor_4.end());

        for (int n=0;n<k;n++)
        {
            observation_buffer(i, m+2+n) = times_factor_4[n];
        }
    }

    return observation;
}

int DRLSNEncoder::type() const {
    return 0;
}

DRLSNEncoder::DRLSNEncoder(string path_to_data) {
    auto [matrix, dim_0, dim_1] = Utils::read_matrix(path_to_data + "/average_hourly_demand.npz");
    demand_forcast = matrix;
}
