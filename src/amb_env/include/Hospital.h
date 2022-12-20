#ifndef AMBULANCE_ENV_HOSPITAL_H
#define AMBULANCE_ENV_HOSPITAL_H


#include <string>

class Hospital {
public:
    explicit Hospital(int hospitalId, std::string name, int position_in_distance_matrix, float lon, float lat);

    /// ID of the hospital
    const int hospital_id;
    const float lon;
    const float lat;

    const std::string name;

    const int position_in_distance_matrix;
};


#endif //AMBULANCE_ENV_HOSPITAL_H
