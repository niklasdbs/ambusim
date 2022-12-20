enable_testing()

add_executable(
        ambulance_env_test
        ambulance_env_test.cpp
)
target_link_libraries(
        ambulance_env_test
        gtest_main
        ambulance_env_lib
        pybind11::module ${PYTHON_LIBRARIES}
)

include(GoogleTest)
gtest_discover_tests(ambulance_env_test)