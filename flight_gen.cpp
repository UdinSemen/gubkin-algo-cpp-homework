#include "flight_gen.h"
#include <vector>

#include <string>


std::vector<flight> generateRandomFlights(int count) {
    std::vector<flight> flights;
    flights.reserve(count);



    const std::vector<std::string> cities = {
        "New York", "Los Angeles", "Chicago", "Miami", "Las Vegas", "Seattle", "Boston"
    };

    const std::vector<float> dist = {
       500.0f,1500.0f,2500.0f,3500.0f,4500.0f,5500.0f,1000.0f,2000.0f,3000.0f,4000.0f,5000.0f
    };

    std::uniform_int_distribution<int> cityDist(0, static_cast<int>(cities.size()) - 1);

    for (int i = 0; i < count; i++) {
        flight f;

        // Completely random distance
        f.distance = dist[i%11];

        // Random weather delay
        f.weather_delay = i%2;

        // Random city
        f.origin_city = cities[i%7];

        flights.push_back(f);
    }

    return flights;
}