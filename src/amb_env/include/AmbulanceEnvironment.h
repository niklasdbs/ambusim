#ifndef AMBULANCE_ENV_AMBULANCEENVIRONMENT_H
#define AMBULANCE_ENV_AMBULANCEENVIRONMENT_H

#include <queue>
#include "Ambulance.h"
#include "BaseStation.h"
#include "events/Event.h"
#include "observation_encoders/ObservationEncoder.h"
#include "dispatch_policies/DispatchPolicy.h"
#include "dispatch_policies/DispatchfromNearestStation.h"
#include "hospital_policies/HospitalPolicy.h"
#include "DistanceCalculator.h"
#include "Utils.h"
#include "Metrics.h"
#include "state_encoder/StateEncoderForRender.h"

struct compare_event_pointers : public std::binary_function<Event *, Event *, bool> {
    bool operator()(const Event *x, const Event *y) const
    { return x->event_time > y->event_time;  }
};

/// The event-driven ambulance environment
class AmbulanceEnvironment {
public:
    explicit AmbulanceEnvironment(string path_to_data, string observation_encoder_to_use, string data_split, bool do_not_shuffle_days,
                         int number_of_ambulances);

    ~AmbulanceEnvironment();

    /// Resets the environment to the next day
    variant<py::array_t<float>, map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState> reset(bool full_reset);
    /// Select an action
    tuple<variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>, float, bool, map<std::string, std::variant<int, float, bool, py::list>>>
    step(int action);

    std::map<std::string, float> get_metrics();

    int number_of_actions;

    /// Get the state object of the environment. May be needed for initialization
    AmbulanceEnvironmentState *get_state();

    variant<vector<int>, map<std::string, vector<int>>> get_observation_shape();
    
    /// return the secondary observation
    variant<py::array_t<float>, map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState> get_secondary_observation();

private:
    /// State of the environment
    AmbulanceEnvironmentState* state;
    /// Observation encoder
    ObservationEncoder* observation_encoder;
    /// Dispatch policy
    DispatchPolicy* dispatch_policy;
    /// Hospital policy
    HospitalPolicy *hospital_policy;
    /// Distance calculator
    DistanceCalculator* dist;
    /// for env renderer in py
    StateEncoderForRender* current_state_encoder;

    /// Episodes to replay. Could be days, could be years, ...
    vector<int> episodes_in_data;

    /// Max times for each episode group key: episode group: value: max time
    map<int, int> episode_groups_max_times;

    /// Comparator for events
    compare_event_pointers event_comp;

    /// Contains an event queue (heap) for every episode (e.g. day, year) (do not modify this queues after creation)
    std::vector<std::vector<Event*> *> event_queues;

    /// Event queue (heap) for the current episode group (e.g. day, year) (can be modified) and will be copied by at the start of each episode
    std::vector<Event*> current_event_queue;

    /// Keep track of various metrics
    Metrics metrics;

    /// Keep a copy of all the episodes to replay. This is required for non shuffle replay.
    vector<int> episodes_in_data_const;


    bool shuffle_episodes = true;

    int pickup_threshold = 60 * 10;
    int number_of_patients_picked_up_below_threshold;
    int number_of_incidents_above_threshold;
    int response_times_above_threshold;

    /// Observation encoder for secondary observation (used for imitation learning)
    ObservationEncoder* secondary_observation_encoder;
    /// temporary store the secondarry observation
    variant<py::array_t<float>, map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState> secondary_observation;

    /// Method for filling the event queue at the beginning
    void fill_event_queue(string path_to_data, string data_split);

    /// Method for initial allocation of ambulances to base stations (uniform distribution)
    void init_locate_ambulances();

    /// This method advances the environment until the next action needs to be taken; return true if done
    bool handle_next_events();

    bool handle_event(Event *event);

    /// Load the incidents, hospitals, stations
    void load_data_set(string path_to_data, int number_of_ambulances);

    void dispatch_ambulance(int current_time, const IncidentEvent *incident_event, Ambulance *ambulance_to_dispatch);

    /// Set certain time related fields in the state using an incident event (thus they may be slightly off at certain times)
    void set_time_related_state_fields_from_incident_event(IncidentEvent *incident_event);
};


#endif //AMBULANCE_ENV_AMBULANCEENVIRONMENT_H
