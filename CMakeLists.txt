cmake_minimum_required(VERSION 3.30)
project(DatasetReading)

set(CMAKE_CXX_STANDARD 26)

include_directories(include)

add_executable(DatasetReading 
        flight.h
        flight.cpp  "Reading_for_search.h" "reading_for_search.cpp")
