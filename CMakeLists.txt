cmake_minimum_required(VERSION 3.0.0)

project(RealDashCanServerQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5DBus)

# Source location
set(main_cpp_base ./src)

# Source files
set(src_main
    "${main_cpp_base}/main.cpp"
    "${main_cpp_base}/RealDashCanTcpServer.h"
    "${main_cpp_base}/RealDashCanTcpServer.cpp"
)

add_executable(${PROJECT_NAME} ${src_main})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::DBus)

