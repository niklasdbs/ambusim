set(CMAKE_CXX_STANDARD 20)

file(GLOB HEADER_LIST "../include/*/*.h" "../include/*.h")
file(GLOB SOURCE_LIST "../src/*/*.cpp" "../src/*.cpp")

# IDEs should put the headers in a nice place
source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})

add_library(ambulance_env_lib STATIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(ambulance_env_lib PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(ambulance_env_lib pybind11::module cnpy)