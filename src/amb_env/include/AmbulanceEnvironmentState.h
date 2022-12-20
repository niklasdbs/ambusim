#ifndef AMBULANCE_ENV_AMBULANCEENVIRONMENTSTATE_H
#define AMBULANCE_ENV_AMBULANCEENVIRONMENTSTATE_H


#include <vector>
#include "Ambulance.h"
#include "BaseStation.h"
#include "Hospital.h"
#include "events/IncidentEvent.h"
#include <pybind11/pybind11.h>

/// This class represents the state of the environment
class AmbulanceEnvironmentState {
public:
    AmbulanceEnvironmentState(int num_of_ambulances, int num_of_base_stations, int num_of_hospitals);
    ~AmbulanceEnvironmentState();
    /// Total number of ambulances
    int number_of_ambulances;
    /// Total number of base stations
    int number_of_base_stations;
    /// Total number of hospitals
    int number_of_hospitals;
    /// Current time of the environment
    int current_time = 0;
    /// Time of the last action beginning
    int action_start_time = 0;

    //define max time of an episode. This corresponds to the end of an episode (whole day, whole year).
    int max_time = 60*60*24;

    //TODO currently certain date/time related fields (weekday, month, year, ...) are only set through incident events and thus may be slightly off
    /// Current day of year
    int current_day_of_year = 0;

    /// Current day of week
    int current_weekday = 0;

    /// Current month
    int current_month = 0;

    /// Current year
    int current_year = 0;

    /// Index of the current day in year
    int current_episode_group_index = 0;

    /// Current ambulance to redeploy
    Ambulance* ambulance_to_redeploy = nullptr;


    /// Array that contains all ambulances (position is identified by ambulance id)
    std::vector<Ambulance*> ambulances;

    /// Array that contains all base stations (position is identified by base station id)
    std::vector<BaseStation*> base_stations;

    /// Array that contains all hospitals (position is identified by base station id)
    std::vector<Hospital*> hospitals;

    /// There might be some situations where there are more incidents than ambulances => put the incidents in the queue
    std::list<IncidentEvent*> incident_queue;

    /// Stores all incidents that happened since the last redeployment
    std::vector<IncidentEvent*> incidents_since_last_action;
};


#endif //AMBULANCE_ENV_AMBULANCEENVIRONMENTSTATE_H
