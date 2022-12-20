#ifndef AMBULANCE_ENV_UTILS_H
#define AMBULANCE_ENV_UTILS_H
#include <string>
#include <vector>

using namespace std;


class Utils {
public:
    Utils();

    static tuple<vector<vector<string>>, int> read_csv(const string &file_path);
    int convert_to_second(string hour, string min, string sec);

    static tuple<double **, int, int> read_distance_matrix(string file_path);

    static float average(const vector<int> &v);
    static tuple<float **, int, int> read_matrix(string file_path);
};


#endif //AMBULANCE_ENV_UTILS_H
