#ifndef DATASETREADING_FLIGHT_ORGANIZER_H
#define DATASETREADING_FLIGHT_ORGANIZER_H

#include "flight.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <map>

class FlightOrganizer {
public:
    bool add_flight(const flight& f);
    const std::unordered_set<flight>& get_all_unique_flights() const;
    std::vector<flight> get_flights_by_aircraft(const std::string& carrier_id, float flight_number) const;
    std::vector<flight> get_flights_by_carrier(const std::string& carrier_id) const;
    size_t get_unique_flights_count() const;
    void save_to_csv(const std::string& filename) const;
    void organize_by_aircraft();

    void clear_all_structures();
    void add_flight_to_all(const flight& f);

    template<typename Container>
    void add_to_container(Container& container, const flight& f);

    template<typename Container>
    const flight* find_in_container(const Container& container, const std::string& key) const;

    template<typename MapContainer>
    std::vector<const flight*> find_in_multimap_container(const MapContainer& container, const std::string& key) const;

    const std::vector<flight>& get_vector_flights() const { return vector_flights; }
    const std::unordered_set<flight>& get_unordered_set_flights() const { return unordered_set_flights; }
    const std::set<flight>& get_set_flights() const { return set_flights; }
    const std::unordered_map<std::string, flight>& get_unordered_map_flights() const { return unordered_map_flights; }
    const std::map<std::string, flight>& get_map_flights() const { return map_flights; }
    const std::unordered_multimap<std::string, flight>& get_unordered_multimap_flights() const { return unordered_multimap_flights; }
    const std::multimap<std::string, flight>& get_multimap_flights() const { return multimap_flights; }

private:
    std::unordered_set<flight> unique_flights;
    std::unordered_map<std::string, std::vector<flight*>> aircraft_to_flights;
    std::string get_aircraft_key(const flight& f) const;

    std::vector<flight> vector_flights;
    std::unordered_set<flight> unordered_set_flights;
    std::set<flight> set_flights;
    std::unordered_map<std::string, flight> unordered_map_flights;
    std::map<std::string, flight> map_flights;
    std::unordered_multimap<std::string, flight> unordered_multimap_flights;
    std::multimap<std::string, flight> multimap_flights;
};

template<typename Container>
void FlightOrganizer::add_to_container(Container& container, const flight& f) {
    if constexpr (std::is_same_v<Container, std::vector<flight>>) {
        container.push_back(f);
    }
    else if constexpr (std::is_same_v<Container, std::unordered_set<flight>> ||
        std::is_same_v<Container, std::set<flight>>) {
        container.insert(f);
    }
    else if constexpr (std::is_same_v<Container, std::unordered_map<std::string, flight>> ||
        std::is_same_v<Container, std::map<std::string, flight>>) {
        container[f.get_unique_key()] = f;
    }
    else if constexpr (std::is_same_v<Container, std::unordered_multimap<std::string, flight>> ||
        std::is_same_v<Container, std::multimap<std::string, flight>>) {
        container.insert({ f.get_unique_key(), f });
    }
}

template<typename Container>
const flight* FlightOrganizer::find_in_container(const Container& container, const std::string& key) const {
    if constexpr (std::is_same_v<Container, std::vector<flight>>) {
        for (const auto& f : container) {
            if (f.get_unique_key() == key) {
                return &f;
            }
        }
    }
    else if constexpr (std::is_same_v<Container, std::unordered_set<flight>> ||
        std::is_same_v<Container, std::set<flight>>) {
        for (const auto& f : container) {
            if (f.get_unique_key() == key) {
                return &f;
            }
        }
    }
    else if constexpr (std::is_same_v<Container, std::unordered_map<std::string, flight>>) {
        auto it = container.find(key);
        if (it != container.end()) {
            return &it->second;
        }
    }
    else if constexpr (std::is_same_v<Container, std::map<std::string, flight>>) {
        auto it = container.find(key);
        if (it != container.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

template<typename MapContainer>
std::vector<const flight*> FlightOrganizer::find_in_multimap_container(const MapContainer& container, const std::string& key) const {
    std::vector<const flight*> result;
    auto range = container.equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(&it->second);
    }
    return result;
}

#endif //DATASETREADING_FLIGHT_ORGANIZER_H
