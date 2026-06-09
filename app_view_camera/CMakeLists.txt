cmake_minimum_required(VERSION 3.5)
project(app_view_camera)

find_package(OpenCV)

add_executable(view_camera ./src/main.cpp)
target_link_libraries(view_camera ufr ${OpenCV_LIBS})
install(TARGETS view_camera DESTINATION bin)
