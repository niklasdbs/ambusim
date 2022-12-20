#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include "../extern/cnpy/cnpy.h"
#include "../include/Utils.h"

using namespace std;

Utils::Utils()= default;

std::tuple<vector<vector<string>>, int> Utils::read_csv(const string &file_path) {
    ifstream file(file_path);
    string line;
    string cell;
    vector<vector<string>> objects;
    if (!file.is_open()) {
        throw ifstream::failure("fail to open file(path:" + file_path + ")");
    }
    getline(file, line);
    int number_of_lines = 0;
    while (getline(file, line)) {
        istringstream line_s(line);
        vector<string> object;
        while (getline(line_s, cell, ',')) {
            object.push_back(cell);
        }

        // This checks for a trailing comma with no data after it.
        if (!line_s && cell.empty())
        {
            // If there was a trailing comma then add an empty element.
            object.push_back("");
        }

        objects.push_back(object);

        number_of_lines++;
    }
    file.close();
    return {objects, number_of_lines};
}

int Utils::convert_to_second(string hour, string min, string sec) {
    int hh = stoi(hour);
    int mm = stoi(min);
    int ss = stoi(sec);
    return 3600*hh + 60*mm + ss;
}


tuple<double **, int, int> Utils::read_distance_matrix(std::string file_path){
    cnpy::npz_t my_npz = cnpy::npz_load(file_path);
    cnpy::NpyArray road_net_np = my_npz["arr_0"];

    int road_net_dim_0 = road_net_np.shape[0];
    int road_net_dim_1 = road_net_np.shape[1];
    auto continous_rd_net = road_net_np.data<double>();
    double ** road_net = new double* [road_net_dim_0];
    for(int i = 0; i< road_net_dim_0; i++)
    {
        road_net[i] = new double[road_net_dim_1];
        memcpy(road_net[i], &continous_rd_net[i*road_net_dim_1] , road_net_dim_1 * sizeof(double ));

    }

    return {road_net, road_net_dim_0, road_net_dim_1};
}

 tuple<float **, int, int> Utils::read_matrix(std::string file_path){
    cnpy::npz_t my_npz = cnpy::npz_load(file_path);
    cnpy::NpyArray np_matrix = my_npz["arr_0"];

    int dim_0 = np_matrix.shape[0];
    int dim_1 = np_matrix.shape[1];
    auto continous_matrix = np_matrix.data<double>();
    float ** matrix = new float * [dim_0];
    for(int i = 0; i< dim_0; i++)
    {
        matrix[i] = new float[dim_1];
        for (int j = 0; j < dim_1; j++)
        {
            matrix[i][j] = static_cast<float>(continous_matrix[i*dim_1 + j]);
        }
    }

    return {matrix, dim_0, dim_1};
}


float Utils::average(std::vector<int> const& v){
    if(v.empty()){
        return 0;
    }

    auto const count = static_cast<float>(v.size());
    return static_cast<float>(std::reduce(v.begin(), v.end())) / count;
}