#include "Metrics.h"
#include "Utils.h"

void Metrics::add_response_time(int response_time) {
    response_times.push_back(response_time);
}

void Metrics::reset() {
    response_times.clear();
    number_of_incidents = 0;
}

std::map<std::string, float> Metrics::get_metrics() {
    //todo add more metrics

    float pickup_within_8min = Metrics::pickup_within_threshold_time(8*60);
    float pickup_within_10min = Metrics::pickup_within_threshold_time(10*60);
    float pickup_within_12min = Metrics::pickup_within_threshold_time(12*60);
    float pickup_within_20min = Metrics::pickup_within_threshold_time(20*60);

    int response_time_95_quantile = get_nth_quantiale(0.95);
    int response_time_90_quantile = get_nth_quantiale(0.9);
    int response_time_80_quantile = get_nth_quantiale(0.8);
    int response_time_50_quantile = get_nth_quantiale(0.5);

    float average_response_time = Utils::average(response_times);

    return {
        {"number_of_incidents", number_of_incidents},
        {"average_response_time", average_response_time},
        {"pickup_within_8min", pickup_within_8min},
        {"pickup_within_10min", pickup_within_10min},
        {"pickup_within_12min", pickup_within_12min},
        {"pickup_within_20min", pickup_within_20min},
        {"response_time_95_quantile", response_time_95_quantile},
        {"response_time_90_quantile", response_time_90_quantile},
        {"response_time_80_quantile", response_time_80_quantile},
        {"response_time_50_quantile", response_time_50_quantile},
    };
}

int Metrics::get_nth_quantiale(float quantile) {
    // calculate quantile in O(n)

    auto nth = response_times.begin();
    const size_t pos = quantile * distance(response_times.begin(), response_times.end());
    advance(nth, pos);
    nth_element(response_times.begin(), nth, response_times.end());
    return *nth;
}

float Metrics::pickup_within_threshold_time(int threshold) {
    float within_threshold = std::count_if(response_times.begin(), response_times.end(), [&](const auto &item) {
        return item <= threshold;
    });

    float pickup_within_threshold_time = within_threshold/response_times.size();

    return pickup_within_threshold_time;
}
