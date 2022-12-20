#ifndef AMBULANCE_ENVP_METRICS_H
#define AMBULANCE_ENVP_METRICS_H


#include <map>
#include <string>
#include <vector>

class Metrics {

public:
    void add_response_time(int response_time);

    int number_of_incidents = 0;

    std::map<std::string, float> get_metrics();

    void reset();

private:
    std::vector<int> response_times;

    float pickup_within_threshold_time(int threshold);

    int get_nth_quantiale(float quantile);
};

#endif //AMBULANCE_ENVP_METRICS_H
