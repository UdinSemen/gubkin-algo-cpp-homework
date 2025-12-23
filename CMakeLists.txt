cmake_minimum_required(VERSION 3.20)
project(FlightDataAnalysis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")  # Максимальная оптимизация

# Включаем директории с заголовочными файлами
include_directories(${CMAKE_SOURCE_DIR}/include)

# Собираем все исходные файлы
set(SOURCES
    src/main.cpp
    src/flight.cpp
    src/flight_organizer.cpp
    src/reading_by_instances.cpp
    src/sorting.cpp
    src/encryption.cpp
    src/compression.cpp
    src/graph.cpp
)

# Основной исполняемый файл
add_executable(FlightAnalysis ${SOURCES})

# Опционально: добавить поддержку многопоточности (для будущей оптимизации)
find_package(Threads)
if(Threads_FOUND)
    target_link_libraries(FlightAnalysis ${CMAKE_THREAD_LIBS_INIT})
endif()
