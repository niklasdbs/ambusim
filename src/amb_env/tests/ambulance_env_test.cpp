#include <gtest/gtest.h>
#include "AmbulanceEnvironment.h"

class AmbulanceEnvTest : public ::testing::Test{
public:
    explicit AmbulanceEnvTest() {
        env = new AmbulanceEnvironment("/ambdata", //todo replace with dummy data path
                                       "NoneNumpyObservationEncoder",
                                       "train",
                                       true,
                                       42);
    }

    AmbulanceEnvironment* env;

protected:
    void SetUp() override {
    }

};

TEST_F(AmbulanceEnvTest, BasicTest){
    env->reset(true);
}
