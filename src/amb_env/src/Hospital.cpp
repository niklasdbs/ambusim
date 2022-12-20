#include "../include/Hospital.h"


Hospital::Hospital(int hospitalId, std::string name, int position_in_distance_matrix, float lon, float lat)
        : hospital_id(hospitalId),
          name(name),
          position_in_distance_matrix(position_in_distance_matrix), lon(lon), lat(lat) {

}
