cmake_minimum_required(VERSION 3.5)
project(app_microfone)

add_executable(app_microfone ./src/main.c)
target_link_libraries(app_microfone asound ufr)
install(TARGETS app_microfone DESTINATION bin)