#ifndef AMBULANCE_ENV_EVENT_H
#define AMBULANCE_ENV_EVENT_H

#include <string>

using namespace std;

enum EventType{incident_event, ambulance_event, ambulance_on_scene_finished_event};

/// Base class for all events in the event-driven simulation.
class Event {
public:
    explicit Event(int eventTime, EventType event_type);
    const int event_time;
    const EventType event_type;
};


#endif //AMBULANCE_ENV_EVENT_H
