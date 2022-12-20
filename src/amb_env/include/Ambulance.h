#ifndef AMBULANCE_ENV_AMBULANCE_H
#define AMBULANCE_ENV_AMBULANCE_H

#include "BaseStation.h"
#include "Hospital.h"
#include "events/IncidentEvent.h"

class BaseStation;
class Hospital;

/// Describes the current state of an ambulance
enum AmbulanceStatus {idle,
                      traveling_to_incident,
                      at_incident,
                      traveling_to_hospital,
                      at_hospital,
                      traveling_to_new_base_station};

/// This class defines an ambulance
class Ambulance {

public:
    explicit Ambulance(int ambulanceId);

public:
    /// ID of an ambulance (should correspond to the position of the ambulance in the array)
    const int ambulance_id;
    /// Current state the ambulance
    AmbulanceStatus status = idle;
    /// Base station of the ambulance (can be null)
    BaseStation* base_station;

    /// Base station which the ambulance is currently heading to (can be null)
    BaseStation* target_base_station;

    /// Hospital currently located/traveling to
    Hospital* current_hospital = nullptr;
    /// Expected arrival time at hospital
    int expected_arrival_time_at_hospital = 0;

    const IncidentEvent* current_incident_event = nullptr;

    bool operator==(const Ambulance& other) const;

    /// This method removes an ambulance from its current base station
    void remove_from_base_station();
};


#endif //AMBULANCE_ENV_AMBULANCE_H
