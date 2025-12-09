#include "flight_organizer.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;

// Вспомогательная функция для парсинга уникального ключа
void parse_unique_key(const string& key, string& carrier, string& flight_num,
    int& year, int& month, int& day, string& origin, string& dest) {
    // Формат ключа: AA_123_2024-01-15_JFK_LAX
    size_t pos1 = key.find('_');
    size_t pos2 = key.find('_', pos1 + 1);
    size_t pos3 = key.find('_', pos2 + 1);
    size_t pos4 = key.find('_', pos3 + 1);

    carrier = key.substr(0, pos1);
    flight_num = key.substr(pos1 + 1, pos2 - pos1 - 1);

    string date = key.substr(pos2 + 1, pos3 - pos2 - 1);
    size_t dash1 = date.find('-');
    size_t dash2 = date.find('-', dash1 + 1);
    year = stoi(date.substr(0, dash1));
    month = stoi(date.substr(dash1 + 1, dash2 - dash1 - 1));1
    day = stoi(date.substr(dash2 + 1));

    origin = key.substr(pos3 + 1, pos4 - pos3 - 1);
    dest = key.substr(pos4 + 1);
}

bool FlightOrganizer::add_flight(const flight& f) {
    auto result = unique_flights.insert(f);
    return result.second;
}

const unordered_set<flight>& FlightOrganizer::get_all_unique_flights() const {
    return unique_flights;
}

string FlightOrganizer::get_aircraft_key(const flight& f) const {
    // Парсим из уникального ключа
    string carrier, flight_num, origin, dest;
    int year, month, day;
    parse_unique_key(f.get_unique_key(), carrier, flight_num, year, month, day, origin, dest);

    return carrier + "_" + flight_num;
}

vector<flight> FlightOrganizer::get_flights_by_aircraft(const string& carrier_id, float flight_number) const {
    vector<flight> result;
    string target_key = carrier_id + "_" + to_string(static_cast<int>(flight_number));

    for (const auto& f : unique_flights) {
        if (get_aircraft_key(f) == target_key) {
            result.push_back(f);
        }
    }
    return result;
}

vector<flight> FlightOrganizer::get_flights_by_carrier(const string& carrier_id) const {
    vector<flight> result;

    for (const auto& f : unique_flights) {
        // Парсим carrier_id из уникального ключа
        string key = f.get_unique_key();
        size_t pos = key.find('_');
        string carrier_from_key = key.substr(0, pos);

        if (carrier_from_key == carrier_id) {
            result.push_back(f);
        }
    }
    return result;
}

size_t FlightOrganizer::get_unique_flights_count() const {
    return unique_flights.size();
}

void FlightOrganizer::save_to_csv(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << endl;
        return;
    }

    // Заголовок CSV
    file << "unique_key,carrier,flight_num,year,month,day,origin,dest\n";

    for (const auto& f : unique_flights) {
        string key = f.get_unique_key();

        // Парсим данные из уникального ключа
        string carrier, flight_num, origin, dest;
        int year, month, day;
        parse_unique_key(key, carrier, flight_num, year, month, day, origin, dest);

        // Записываем в CSV
        file << key << ","
            << carrier << ","
            << flight_num << ","
            << year << ","
            << month << ","
            << day << ","
            << origin << ","
            << dest << "\n";
    }

    file.close();
    cout << "Saved " << unique_flights.size() << " flights to " << filename << endl;
}

void FlightOrganizer::organize_by_aircraft() {
    aircraft_to_flights.clear();

    for (const auto& f : unique_flights) {
        string key = get_aircraft_key(f);
        aircraft_to_flights[key].push_back(const_cast<flight*>(&f));
    }
}