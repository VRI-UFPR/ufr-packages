cmake_minimum_required(VERSION 3.5)
project(iris_lama_ros)

# Default to release mode.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(iris_lama REQUIRED)
find_package(yaml-cpp REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Specify additional locations of header files
include_directories(include ${YAML_CPP_INCLUDE_DIR})
# add_subdirectory(src)

add_executable(graph_slam2d src/graph_slam2d.cpp)
target_link_libraries(graph_slam2d iris_lama::iris_lama ufr)


# Install our lannch files
# install(DIRECTORY launch/
#     DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
#    PATTERN ".svn" EXCLUDE
# )


