cmake_minimum_required(VERSION 3.5)
project(app_orbbec_proxy)

# Example 1: simple
add_executable(app_orbbec_proxy ./src/main.c)
target_link_libraries(app_orbbec_proxy ufr)
# install(TARGETS app_orbbec_pub DESTINATION bin)
