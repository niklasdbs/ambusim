#include <cassert>
#include "../include/Ambulance.h"

Ambulance::Ambulance(int ambulanceId) : ambulance_id(ambulanceId), base_station(nullptr) {
}

bool Ambulance::operator==(const Ambulance &other) const {
    return this->ambulance_id == other.ambulance_id;
}

void Ambulance::remove_from_base_station() {
    assert(("no base station assigned", base_station != nullptr));
    base_station->remove_ambulance(*this);
    base_station = nullptr;
}
