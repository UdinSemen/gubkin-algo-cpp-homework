//
// Created by Дмитрий Егорычев
//

#ifndef DATASETREADING_FLIGHT_H
#define DATASETREADING_FLIGHT_H
#pragma once
#include <string>

class flight {
public:
    explicit flight(const std::string &parts);
    void print();

private:
    int year{};
    int month{};
    int month_day{};
    int week_day{};
    std::string carrier_id;
    float flight_number{};
    std::string origin_code;
    std::string origin_city;
    std::string origin_state;
    std::string dest_code;
    std::string dest_city;
    std::string dest_state;
    int crs_dep_time{};
    float dep_time{};
    float dep_delay{};
    float taxi_out{};
    float wheels_off{};
    float wheels_on{};
    float taxi_in{};
    int crs_arr_time{};
    float arr_time{};
    float arr_delay{};
    bool canceled{};
    char cancellation_code{};
    bool diverted{};
    float crs_elapsed{};
    float actual_elapsed{};
    float air_time{};
    float distance{};
    bool carrier_delay{};
    bool weather_delay{};
    bool nas_delay{};
    bool security_delay{};
    bool late_aircraft_delay{};
};


#endif //DATASETREADING_FLIGHT_H
