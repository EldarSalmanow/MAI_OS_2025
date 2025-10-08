cmake_minimum_required(VERSION 3.23)
project(MAI_OS_2025)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(cmake/Utils.cmake)
setup_project()

add_subdirectory(lab1)
add_subdirectory(lab2)
