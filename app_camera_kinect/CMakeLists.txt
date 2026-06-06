cmake_minimum_required(VERSION 3.10)
project(app_camera_kinect)

# Defina a versão do OpenCV que você deseja usar
find_package(OpenCV REQUIRED)

# Encontre a biblioteca Freenect
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(FREENECT REQUIRED freenect)

# Adicione as flags para o compialador
include_directories(${OpenCV_INCLUDE_DIRS})

# Defina o arquivo executável
add_executable(camera_kinect src/main.cpp)

# Vincule o OpenCV e a Freenect
target_link_libraries(camera_kinect ${OpenCV_LIBS} freenect ufr)