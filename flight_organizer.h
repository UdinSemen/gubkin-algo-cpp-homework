#pragma once
#ifndef DATASETREADING_FLIGHT_ORGANIZER_H
#define DATASETREADING_FLIGHT_ORGANIZER_H

#include "flight.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

class FlightOrganizer {
public:
    bool add_flight(const flight& f);
    const std::unordered_set<flight>& get_all_unique_flights() const;
    std::vector<flight> get_flights_by_aircraft(const std::string& carrier_id, float flight_number) const;
    std::vector<flight> get_flights_by_carrier(const std::string& carrier_id) const;
    size_t get_unique_flights_count() const;
    void save_to_csv(const std::string& filename) const;
    void organize_by_aircraft();

private:
    std::unordered_set<flight> unique_flights;
    std::unordered_map<std::string, std::vector<flight*>> aircraft_to_flights;
    std::string get_aircraft_key(const flight& f) const;
};

#endif //DATASETREADING_FLIGHT_ORGANIZER_H