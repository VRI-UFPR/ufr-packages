################################################################################
# Set minimum required version of cmake, project name and compile options
################################################################################
cmake_minimum_required(VERSION 3.5)
project(lidar_ld08)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(MSVC)
  add_compile_definitions(
    _USE_MATH_DEFINES
  )
endif()

################################################################################
# Build
################################################################################
include_directories(./include)

SET(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")

file(GLOB  MAIN_SRC src/*.cpp)

add_executable(lidar_ld08  ${MAIN_SRC})
target_link_libraries(lidar_ld08 pthread udev ufr)

################################################################################
# Install
################################################################################

install(
  TARGETS lidar_ld08
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)