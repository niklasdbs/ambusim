#include "AmbulanceEnvironment.h"

int main(){
    AmbulanceEnvironment env = AmbulanceEnvironment("EmRL/data/processed/sf",
                                                    "NoneNumpyObservationEncoder",
                                                    "train",
                                                    true,
                                                    42);
    auto obs_0 = env.reset(true);
    while (true) {
        auto [obs, reward, done, infos] = env.step(0);
        if (done) {
            //break;
            env.reset(false);
        }
    }
    return 0;
}
