#ifndef DATASETREADING_FLIGHT_H
#define DATASETREADING_FLIGHT_H

#include <string>
#include <vector>
#include <tuple>

class flight {
public:
    flight();
    void by_instances(const std::string& parts);
    void by_slices(const std::vector<std::string>& parts);
    void print();

    bool operator==(const flight& other) const;
    bool operator<(const flight& other) const;
    std::string get_unique_key() const;

    const std::string& get_carrier_id() const { return carrier_id; }
    float get_flight_number() const { return flight_number; }

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

namespace std {
    template<>
    struct hash<flight> {
        size_t operator()(const flight& f) const {
            return hash<string>()(f.get_unique_key());
        }
    };
}

#endif //DATASETREADING_FLIGHT_H
//
// Created by Дмитрий Егорычев
//

#ifndef DATASETREADING_FLIGHT_H
#define DATASETREADING_FLIGHT_H
#include <string>
#include <vector>

class flight {
public:
    flight();
    void by_instances(const std::string &parts);
    void by_slices(const std::vector<std::string> &parts);
    void print() const;

    const std::string& get_dest_state() const { return dest_state; }
    float get_arr_delay() const { return arr_delay; }
    bool is_canceled() const { return canceled; }
    void setDistance(float d) { distance = d; }
    void setWeatherDelay(bool wd) { weather_delay = wd; }
    void setOriginCity(const std::string& city) { origin_city = city; }
    float getDistance() const { return distance; }
    bool hasWeatherDelay() const { return weather_delay; }
    std::string getOriginCity() const{ return origin_city; }
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
