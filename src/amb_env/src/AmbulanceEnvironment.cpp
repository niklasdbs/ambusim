//Pyind11 connect
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <iostream>
#include <tuple>
#include <cassert>
#include <any>
#include "../include/AmbulanceEnvironment.h"
#include "../include/events/AmbulanceEvent.h"
#include "../include/dispatch_policies/DispatchfromRandomStation.h"
#include "../include/hospital_policies/NearestHospitalPolicy.h"
#include "../include/observation_encoders/SimpleObservationEncoder.h"

#include "../include/observation_encoders/NoneNumpyObservationEncoder.h"
#include "../include/state_encoder/StateEncoderForRender.h"

#include "observation_encoders/StateObjectEncoder.h"
#include "observation_encoders/NearestBaseStationEncoder.h"
#include "observation_encoders/AmbulanceThatNeedsToActEncoder.h"
#include "observation_encoders/DRLSNEncoder.h"
#include "observation_encoders/AmbulanceDestinationEncoder.h"

using namespace std;
#ifdef NDEBUG
#define DEBUG(x)
#else
#define DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#endif

AmbulanceEnvironment::AmbulanceEnvironment(string path_to_data,
                                           string observation_encoder_to_use,
                                           string data_split,
                                           bool do_not_shuffle_days,
                                           int number_of_ambulances) {
    if (observation_encoder_to_use == "SimpleObservationEncoder") {
        observation_encoder = new SimpleObservationEncoder();
    } else if (observation_encoder_to_use == "NoneNumpyObservationEncoder") {
        observation_encoder = new NoneNumpyObservationEncoder();
    } else if (observation_encoder_to_use == "StateObjectEncoder") {
        observation_encoder = new StateObjectEncoder();
    } else if (observation_encoder_to_use == "NearestBaseStationEncoder") {
        observation_encoder = new NearestBaseStationEncoder();
    } else if (observation_encoder_to_use == "AmbulanceThatNeedsToActEncoder") {
        observation_encoder = new AmbulanceThatNeedsToActEncoder();
    } else if (observation_encoder_to_use == "AmbulanceDestinationEncoder") {
        observation_encoder = new AmbulanceDestinationEncoder();
    } else if (observation_encoder_to_use == "DRLSNEncoder") {
        observation_encoder = new DRLSNEncoder(path_to_data);
    } else {
        throw "Unknown observation encoder: " + observation_encoder_to_use;
    }

    secondary_observation_encoder = new NoneNumpyObservationEncoder(path_to_data); //todo do not hardcode!!!

    if (data_split == "val" || data_split == "test" || do_not_shuffle_days) {
        shuffle_episodes = false;
    }

    auto [station_to_incident, _, __] = Utils::read_distance_matrix(path_to_data + "/station_to_incident.npz");
    auto [incident_to_hospital, ___, ____] = Utils::read_distance_matrix(path_to_data + "/incident_to_hospitals.npz");
    auto [hospital_to_station, _____, ______] = Utils::read_distance_matrix(path_to_data + "/hospital_to_station.npz");
    auto [incident_station, _______, ________] = Utils::read_distance_matrix(path_to_data + "/incident_to_station.npz");
    auto [station_to_station, _________,__________] = Utils::read_distance_matrix(path_to_data + "/station_to_station.npz");

    dist = new DistanceCalculator(station_to_incident, incident_station, incident_to_hospital, hospital_to_station, station_to_station);
    dispatch_policy = new DispatchfromNearestStation(dist);
    hospital_policy = new NearestHospitalPolicy(dist);


    load_data_set(path_to_data, number_of_ambulances);

    init_locate_ambulances();
    fill_event_queue(path_to_data, data_split);

    number_of_actions = state->number_of_base_stations;
    current_state_encoder = new StateEncoderForRender(state->number_of_base_stations);
}

AmbulanceEnvironment::~AmbulanceEnvironment() {
    delete state;
    delete dispatch_policy;
    delete observation_encoder;
    delete hospital_policy;
    delete dist;
    delete current_state_encoder;
}


void AmbulanceEnvironment::load_data_set(string path_to_data, int number_of_ambulances) {
    auto [base_stations, number_of_base_stations] = Utils::read_csv(path_to_data + "/stations.csv");
    auto [hospitals, number_of_hospitals] = Utils::read_csv(path_to_data + "/hospitals.csv");

    state = new AmbulanceEnvironmentState(number_of_ambulances,
                                          number_of_base_stations,
                                          number_of_hospitals);

    for (int i = 0; i < number_of_base_stations; i++) {
        state->base_stations[i] = new BaseStation(stoi(base_stations[i][0]),
                                                  base_stations[i][1],
                                                  stoi(base_stations[i][2]),
                                                  stof(base_stations[i][3]),
                                                  stof(base_stations[i][4]));
    }
    for (int i = 0; i < number_of_hospitals; i++) {
        state->hospitals[i] = new Hospital(stoi(hospitals[i][0]),
                                           hospitals[i][1],
                                           stoi(hospitals[i][2]),
                                           stof(hospitals[i][3]),
                                           stof(hospitals[i][4]));
    }

    for (int i = 0; i < state->number_of_ambulances; i++) {
        auto *ambulance = new Ambulance(i);
        state->ambulances[i] = ambulance;
    }


}

void AmbulanceEnvironment::init_locate_ambulances() {
    for (int i = 0; i < state->number_of_ambulances; i++) {
        int base_station_index = i % state->number_of_base_stations;
        state->base_stations[base_station_index]->add_ambulance(*state->ambulances[i]);
        state->base_stations[base_station_index]->number_of_idle_ambulances++;
    }
}


void AmbulanceEnvironment::fill_event_queue(string path_to_data, string data_split) {
    //todo nicer structure of all the parameters here
    auto [episode_groups_max_times_csv, number_of_episode_groups_max_times] =
            Utils::read_csv(path_to_data + "/episode_groups_" + data_split + ".csv");

    for (auto &ep_group: episode_groups_max_times_csv) {
        int e_group = stoi(ep_group[1]);
        int max_time = stoi(ep_group[2]);
        episode_groups_max_times[e_group] = max_time;
    }

    int episode_group_position_in_csv = 8;

    //input data (given by day, year...)
    auto [incidents, n_of_incidents] = Utils::read_csv(path_to_data + "/" + data_split + ".csv");

    std::set<int> episodes_in_data_set;
    for (auto &i: incidents) {
        episodes_in_data_set.insert(stoi(i[episode_group_position_in_csv]));
    }

    episodes_in_data = std::vector<int>(episodes_in_data_set.begin(), episodes_in_data_set.end());
    episodes_in_data_const = std::vector<int>(episodes_in_data); //create copy for certain replay modes
    int number_of_episode_groups = episodes_in_data.size();

    std::map<int, int> episode_group_mapping;
    int i = 0;
    for (int episode_group: episodes_in_data) {
        episode_group_mapping[episode_group] = i;
        i++;
    }

    for (int i = 0; i < number_of_episode_groups; i++) {
        event_queues.push_back(new std::vector<Event *>());
    }

    for (auto &i: incidents) {
        int day_of_year = stoi(i[2]);
        int episode_group_ident = stoi(i[episode_group_position_in_csv]);
        int position_in_list = episode_group_mapping[episode_group_ident];
        int month = stoi(i[5]);
        int week_day = stoi(i[6]);
        int year = stoi(i[7]);
        float lon = stof(i[9]);
        float lat = stof(i[10]);
        int prio = stoi(i[11]);
        bool transport_to_hospital;
        std::transform (i[12].begin(), i[12].end(), i[12].begin(), ::tolower);
        std::istringstream(i[12]) >> std::boolalpha >> transport_to_hospital;
        int seconds_at_incident = i[13].size() >0? stoi(i[13]) : 0;
        int total_emergency_time = i[14].size()> 0? stoi(i[14]) : 0;
        int time_at_hospital = i[15].size() > 0? stoi(i[15]) : 0;


        event_queues[position_in_list]->push_back(new IncidentEvent(stoi(i[1]),
                                                                    day_of_year,
                                                                    stoi(i[3]),
                                                                    stoi(i[4]),
                                                                    month,
                                                                    week_day,
                                                                    year,
                                                                    prio,
                                                                    lon,
                                                                    lat,
                                                                    transport_to_hospital,
                                                                    seconds_at_incident,
                                                                    time_at_hospital,
                                                                    total_emergency_time));
    }

    for (int i = 0; i < number_of_episode_groups; i++) {
        std::make_heap(event_queues[i]->begin(), event_queues[i]->end(), event_comp);
    }

}

std::variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>
AmbulanceEnvironment::reset(bool full_reset) {
    DEBUG("Reset");
    if (full_reset) {
        //reset the episode groups
        episodes_in_data = vector<int>(episodes_in_data_const);
    }
    //reset all time/date related fields
    state->incidents_since_last_action.clear();
    state->incident_queue.clear();
    state->current_time = 0;
    state->action_start_time = 0;
    state->ambulance_to_redeploy = nullptr;
    for (Ambulance *ambulance: state->ambulances) {
        ambulance->base_station = nullptr;
        ambulance->target_base_station = nullptr;
        ambulance->current_hospital = nullptr;
        ambulance->current_incident_event = nullptr;
        ambulance->status = idle;
    }

    for (BaseStation *base_station: state->base_stations) {
        base_station->ambulances.clear();
        base_station->number_of_idle_ambulances = 0;
    }
    init_locate_ambulances();

    metrics.reset();

    //test and validation should not be chosen random days but instead replay all days sequentially
    int current_episode_group_index;
    if (shuffle_episodes) {
        current_episode_group_index = rand() % episodes_in_data.size();
    } else {
        if (episodes_in_data.empty()) {
            //reset the episode groups
            episodes_in_data = vector<int>(episodes_in_data_const);
        }

        current_episode_group_index = episodes_in_data_const.size() - episodes_in_data.size();
        episodes_in_data.pop_back();
    }

    state->current_episode_group_index = current_episode_group_index;

    state->max_time = episode_groups_max_times[episodes_in_data_const[current_episode_group_index]];

    current_event_queue = std::vector<Event *>(*event_queues[current_episode_group_index]);
    DEBUG("Current Episode Group index: " << current_episode_group_index);

    handle_next_events();

    auto observation = observation_encoder->encode(*state, *dist);
    if (secondary_observation_encoder!= nullptr)
    {
        secondary_observation = secondary_observation_encoder->encode(*state, *dist);
    }
    state->incidents_since_last_action.clear();
    return observation;
}

/// This function processes all events until the next action needs to be taken.
bool AmbulanceEnvironment::handle_next_events() {
    while (!current_event_queue.empty()) {
        Event *next_event = current_event_queue.front();

        if (next_event->event_time >= state->max_time) {
            return true;
        }

        std::pop_heap(current_event_queue.begin(), current_event_queue.end(), event_comp);
        current_event_queue.pop_back();

        /// handle_event returns the index of amb, which needs to be redeployed
        bool need_to_redeploy = handle_event(next_event);

        if (need_to_redeploy) {
            break;
        }
    }
    if (current_event_queue.empty()) {
        return true;
    } else {
        return false;
    }
}

void AmbulanceEnvironment::set_time_related_state_fields_from_incident_event(IncidentEvent* incident_event){
    state->current_year = incident_event->year;
    state->current_day_of_year = incident_event->day_of_year;
    state->current_month = incident_event->month;
    state->current_weekday = incident_event->weekday;
}

/// This function processes a single event and indicates whether additional events need to be handled or an action needs
/// to be chosen.
/// \param event a single event
/// \return index of the ambulance if it needs to be redeployed
bool AmbulanceEnvironment::handle_event(Event *event) {
    auto current_time = event->event_time;
    //do stuff that needs to be done for all events here
    state->current_time = current_time; //advance time of the environment

    bool need_to_redeploy = false;
    switch (event->event_type) {
        case incident_event: {
            auto* incident_event = static_cast<IncidentEvent*>(event);
            set_time_related_state_fields_from_incident_event(incident_event);
            state->incidents_since_last_action.push_back(incident_event);
            // choose an ambulance to dispatch
            Ambulance *ambulance_to_dispatch = dispatch_policy->dispatch(*state, incident_event);


            if (ambulance_to_dispatch == nullptr) //there is no ambulance available => move incident to queue
            {
                state->incident_queue.push_back(incident_event);
            } else {
                dispatch_ambulance(current_time, incident_event, ambulance_to_dispatch);

            }
            break;
        }
        case ambulance_event: {
            auto* ambulance_event = static_cast<AmbulanceEvent*>(event);
            auto *ambulance = state->ambulances[ambulance_event->ambulance_id];
            switch (ambulance_event->ambulance_event_type) {
                case arrive_at_base:
                    DEBUG("Ambulance " << ambulance_event->ambulance_id << " arrives at new base: "
                                       << ambulance_event->event_time);
                    ambulance->status = idle;
                    ambulance->target_base_station->number_of_idle_ambulances++;
                    ambulance->current_hospital = nullptr;
                    ambulance->target_base_station = nullptr;
                    ambulance->current_incident_event = nullptr;

                    if (!state->incident_queue.empty()) {
                        IncidentEvent *incident_event = state->incident_queue.front();
                        state->incident_queue.pop_front();
                        // choose an ambulance to dispatch
                        Ambulance *ambulance_to_dispatch = dispatch_policy->dispatch(*state, incident_event);

                        if (ambulance_to_dispatch != nullptr) {
                            dispatch_ambulance(current_time, incident_event, ambulance_to_dispatch);
                        }
                    }

                    break;
                case arrive_at_incident:
                    ambulance->status = at_incident;
                    break;
                case arrive_at_hospital:
                    state->ambulances[ambulance_event->ambulance_id]->status = at_hospital;
                    break;
                case finished_at_hospital:
                    need_to_redeploy = true;
                    state->ambulance_to_redeploy = state->ambulances[ambulance_event->ambulance_id];
                    break;
            }


            break;
        }
        case ambulance_on_scene_finished_event:
        {
            auto *scene_finished_event = static_cast<AmbulanceOnSceneFinishedEvent *>(event);
            bool travel_to_hospital = scene_finished_event->travel_to_hospital;
            auto * ambulance = state->ambulances[scene_finished_event->ambulance_id];

            if (travel_to_hospital) {
                //convey : from incident site to hospital
                ambulance->status = traveling_to_hospital;

                //choose a hospital to convey patient to
                Hospital &dest_hospital = hospital_policy->find_hospital(*(scene_finished_event->incident_event),
                                                                         *state);
                // calculate travel time
                int travel_time_to_hospital = dist->travel_time(scene_finished_event->incident_event,
                                                                &dest_hospital);
                //create new ambulance event
                AmbulanceEvent *ambulance_arrives_at_hospital = new AmbulanceEvent(
                        current_time + travel_time_to_hospital,
                        scene_finished_event->ambulance_id,
                        arrive_at_hospital);
                //add the event to the queue
                current_event_queue.push_back(ambulance_arrives_at_hospital);
                push_heap(current_event_queue.begin(), current_event_queue.end(), event_comp);

                ambulance->current_hospital = &dest_hospital;
                ambulance->expected_arrival_time_at_hospital = ambulance_arrives_at_hospital->event_time;

                //spend some time at the hospital
                int time_at_hospital = scene_finished_event->incident_event->time_at_hospital;
                if (time_at_hospital <= 0){
                    time_at_hospital = 1;//ensure that finished at hospital event comes at the correct time
                }

                //create new ambulance event
                AmbulanceEvent *ambulance_finished_at_hospital = new AmbulanceEvent(
                        current_time + travel_time_to_hospital + time_at_hospital,
                        scene_finished_event->ambulance_id,
                        finished_at_hospital);

                //add the event to the queue
                current_event_queue.push_back(ambulance_finished_at_hospital);
                push_heap(current_event_queue.begin(), current_event_queue.end(), event_comp);

            } else {
                need_to_redeploy = true;
                state->ambulance_to_redeploy = ambulance;

            }

            break;
        }
    }

    return need_to_redeploy;
}

void AmbulanceEnvironment::dispatch_ambulance(int current_time, const IncidentEvent *incident_event,
                                              Ambulance *ambulance_to_dispatch) {/*
                                               * 1. dispatch : from base_station to incident site
                                               */
    DEBUG("Ambulance " << ambulance_to_dispatch->ambulance_id << " dispatched: " << incident_event->event_time);
    ambulance_to_dispatch->status = traveling_to_incident;
    // calculate travel time
    int travel_time_to_incident = dist->travel_time(ambulance_to_dispatch->base_station, incident_event);

    //remove the ambulance from the base station it is currently assigned to
    ambulance_to_dispatch->remove_from_base_station();
    ambulance_to_dispatch->current_incident_event = incident_event;

    //create new ambulance event
    AmbulanceEvent *ambulance_arrives_at_incident = new AmbulanceEvent(current_time + travel_time_to_incident,
                                                                       ambulance_to_dispatch->ambulance_id,
                                                                       arrive_at_incident);
    //add the event to the queue
    current_event_queue.push_back(ambulance_arrives_at_incident);
    push_heap(current_event_queue.begin(), current_event_queue.end(), event_comp);

    int seconds_spent_at_incident = incident_event->seconds_at_incident;
    if (seconds_spent_at_incident <= 0)
    {
        seconds_spent_at_incident = 1; //ensure that at least 1 second is spent at the incident site (otherwise we can not ensure that the on scene finished event is executed after arrive event)
    }

    auto* on_scene_finished_event = new AmbulanceOnSceneFinishedEvent(
            current_time + travel_time_to_incident + seconds_spent_at_incident,
            ambulance_to_dispatch->ambulance_id,
            incident_event,
            incident_event->transport_to_hospital);

    //add the event to the queue
    current_event_queue.push_back(on_scene_finished_event);
    push_heap(current_event_queue.begin(), current_event_queue.end(), event_comp);



    //log metrics
    int response_time = current_time - incident_event->event_time + travel_time_to_incident;
    metrics.number_of_incidents++;
    metrics.add_response_time(response_time);

    if (response_time <= pickup_threshold) {
        number_of_patients_picked_up_below_threshold++;
    }
    else{
        number_of_incidents_above_threshold++;
        response_times_above_threshold += response_time;
    }

}

/// Take an action and advance the environment until the next action needs to be taken.
/// \param action integer id of the base station where to redeploy the current ambulance.
std::tuple<std::variant<py::array_t<float>, std::map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState>,
        float,
        bool,
        std::map<std::string, std::variant<int, float, bool, py::list>>> AmbulanceEnvironment::step(int action) {
    assert(action >= 0);
    assert(action < state->number_of_base_stations);

    state->incidents_since_last_action.clear();
    number_of_patients_picked_up_below_threshold = 0;
    number_of_incidents_above_threshold = 0;
    response_times_above_threshold = 0;
    Ambulance *ambulance_to_redeploy = state->ambulance_to_redeploy;
    // calculate travel time
    int travel_time_to_new_base_station;
    if (ambulance_to_redeploy->status == at_incident) {
        travel_time_to_new_base_station = dist->travel_time(ambulance_to_redeploy->current_incident_event,
                                                            state->base_stations[action]);
    } else {
        assert(ambulance_to_redeploy->status == at_hospital);
        travel_time_to_new_base_station = dist->travel_time(ambulance_to_redeploy->current_hospital,
                                                            state->base_stations[action]);
    }
    //create new ambulance event
    ambulance_to_redeploy->target_base_station = state->base_stations[action];
    ambulance_to_redeploy->target_base_station->add_ambulance(*ambulance_to_redeploy);
    ambulance_to_redeploy->status = traveling_to_new_base_station;
    ambulance_to_redeploy->expected_arrival_time_at_hospital = 0;
    AmbulanceEvent *ambulance_arrives_at_new_base_station = new AmbulanceEvent(
            state->current_time + travel_time_to_new_base_station,
            ambulance_to_redeploy->ambulance_id,
            arrive_at_base);
    current_event_queue.push_back(ambulance_arrives_at_new_base_station);
    std::push_heap(current_event_queue.begin(), current_event_queue.end(), event_comp);

    state->ambulance_to_redeploy = nullptr;

    bool done = handle_next_events();

    int action_duration = state->current_time - state->action_start_time;

    auto current_state = observation_encoder->encode(*state, *dist);
    if (secondary_observation_encoder!= nullptr)
    {
        secondary_observation = secondary_observation_encoder->encode(*state, *dist);
    }

    float reward = static_cast<float>(number_of_patients_picked_up_below_threshold);

    // if (number_of_incidents_above_threshold>0)
    // {
    //     reward += 0.1 * -(response_times_above_threshold / static_cast<float>(number_of_incidents_above_threshold))/3600.0f;
    // }

    bool all_days_done = episodes_in_data.empty();
    std::map<std::string, std::variant<int, float, bool, py::list>> infos;
    if (done) {
        infos = {{"dt",            action_duration},
                 {"all_days_done", all_days_done}};
    } else {
        infos = {{"dt", action_duration}};
    }

    state->action_start_time = state->current_time;

    return {current_state, reward, done, infos};
}

std::map<std::string, float> AmbulanceEnvironment::get_metrics() {
    return metrics.get_metrics();
}

AmbulanceEnvironmentState *AmbulanceEnvironment::get_state() {
    return state;
}

std::variant<vector<int>, std::map<std::string, vector<int>>> AmbulanceEnvironment::get_observation_shape() {
    return observation_encoder->shape(
            *state);
}

variant<py::array_t<float>, map<std::string, py::array_t<float>>, int, AmbulanceEnvironmentState> AmbulanceEnvironment::get_secondary_observation(){
    return secondary_observation;
}