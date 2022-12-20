#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "AmbulanceEnvironment.h"
//#include <pybind11/stl_bind.h>
namespace py = pybind11;

//PYBIND11_MAKE_OPAQUE(std::map<std::string, py::array_t<float>>);


//todo look into opaque binding and check if automatic conversion is fast enough https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html
PYBIND11_MODULE(ambulance_env, m) {
    py::enum_<AmbulanceStatus>(m, "AmbulanceStatus")
            .value("idle", AmbulanceStatus::idle)
            .value("traveling_to_incident", AmbulanceStatus::traveling_to_incident)
            .value("at_incident", AmbulanceStatus::at_incident)
            .value("traveling_to_hospital", AmbulanceStatus::traveling_to_hospital)
            .value("at_hospital", AmbulanceStatus::at_hospital)
            .value("traveling_to_new_base_station", AmbulanceStatus::traveling_to_new_base_station)
            .export_values();

    py::class_<Hospital>(m, "Hospital")
            .def_readonly("name", &Hospital::name);

    py::class_<BaseStation>(m, "BaseStation")
            .def_readonly("base_station_id", &BaseStation::base_station_id)
            .def_readonly("ambulances", &BaseStation::ambulances)
            .def_readonly("number_of_idle_ambulances", &BaseStation::number_of_idle_ambulances);

    py::class_<Ambulance>(m, "Ambulance")
            .def_readonly("ambulance_id", &Ambulance::ambulance_id)
            .def_readonly("base_station", &Ambulance::base_station)
            .def_readonly("status", &Ambulance::status);


    py::class_<AmbulanceEnvironmentState>(m, "AmbulanceEnvironmentState")
            .def_readonly("current_time", &AmbulanceEnvironmentState::current_time)
            .def_readonly("ambulances", &AmbulanceEnvironmentState::ambulances)
            .def_readonly("base_stations", &AmbulanceEnvironmentState::base_stations)
            .def_readonly("incident_queue", &AmbulanceEnvironmentState::incident_queue)
            .def_readonly("hospitals", &AmbulanceEnvironmentState::hospitals);

py::class_<AmbulanceEnvironment>(m, "AmbulanceEnvironment")
.def(py::init<std::string, std::string, std::string,bool, int>())
.def("reset", &AmbulanceEnvironment::reset) //py::return_value_policy::take_ownership
.def("step", &AmbulanceEnvironment::step) //todo huge problem because for some obs we need python to deallocate for others c++ needs to do this
.def("get_state", &AmbulanceEnvironment::get_state, py::return_value_policy::reference)
.def("get_secondary_observation", &AmbulanceEnvironment::get_secondary_observation) //py::return_value_policy::take_ownership
.def("get_metrics", &AmbulanceEnvironment::get_metrics)
.def("get_observation_shape", &AmbulanceEnvironment::get_observation_shape)
.def_readonly("number_of_actions", &AmbulanceEnvironment::number_of_actions);

//        py::bind_map<std::map<std::string, py::array_t<float>>>(m, "ObsMapNp");
}

