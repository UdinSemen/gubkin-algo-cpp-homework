cmake_minimum_required(VERSION 3.30)
project(DatasetReading)

set(CMAKE_CXX_STANDARD 26)

add_executable(DatasetReading main.cpp
        flight.h
        flight.cpp)
