cmake_minimum_required(VERSION 3.5)
project(app_camera_sub)

find_package(OpenCV)

# Example 1: simple
add_executable(app_camera_sub ./src/main.cpp)
target_link_libraries(app_camera_sub ufr ${OpenCV_LIBS})
# install(TARGETS app_pub_camera DESTINATION bin)
