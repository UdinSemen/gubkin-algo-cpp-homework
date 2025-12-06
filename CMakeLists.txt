cmake_minimum_required(VERSION 3.30)
project(DatasetReading)

set(CMAKE_CXX_STANDARD 26)

include_directories(include)

add_executable(DatasetReading reading_by_instances.cpp
        flight.h
        flight.cpp)
