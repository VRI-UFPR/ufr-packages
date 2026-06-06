cmake_minimum_required(VERSION 3.5)
project(app_orbbec_pub)

find_package(OpenCV)

# Configure OpenNI
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
message( STATUS "Architecture: ${ARCHITECTURE}" )
if( ${ARCHITECTURE} STREQUAL "x86_64" )
    include_directories(./openni/x86_64/include)
    add_subdirectory(./openni/x86_64)
else()
    include_directories(./openni/arm64/include)
    add_subdirectory(./openni/arm64)
endif()

# Application orbbec_pub
add_executable(orbbec_pub ./src/main.cpp)
target_link_libraries(orbbec_pub ufr ${OpenCV_LIBS} OpenNI2)
install(TARGETS orbbec_pub DESTINATION bin)
