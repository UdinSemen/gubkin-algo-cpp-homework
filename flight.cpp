//
// Created by Дмитрий Егорычев
//

#include "flight.h"
#include <iostream>
#include <sstream>

using namespace std;


flight::flight() = default;

void flight::by_instances(const string &parts) {
    stringstream ss(parts);
    ss >> year >> month >> month_day >> week_day >> carrier_id >> flight_number
            >> origin_code >> origin_city >> origin_state >> dest_code >> dest_city
            >> dest_state >> crs_dep_time >> dep_time >> dep_delay >> taxi_out
            >> wheels_off >> wheels_on >> taxi_in >> crs_arr_time >> arr_time
            >> arr_delay >> canceled >> cancellation_code >> diverted
            >> crs_elapsed >> actual_elapsed >> air_time >> distance >> carrier_delay
            >> weather_delay >> nas_delay >> security_delay >> late_aircraft_delay;
}

void flight::by_slices(const vector<string> &parts) {
    year = stoi(parts[0]);
    month = stoi(parts[1]);
    month_day = stoi(parts[2]);
    week_day = stoi(parts[3]);
    carrier_id = parts[4];
    flight_number = stof(parts[5]);
    origin_code = parts[6];
    origin_city = parts[7];
    origin_state = parts[8];
    dest_code = parts[9];
    dest_city = parts[10];
    dest_state = parts[11];
    crs_dep_time = stoi(parts[12]);
    dep_time = stof(parts[13]);
    dep_delay = stof(parts[14]);
    taxi_out = stof(parts[15]);
    wheels_off = stof(parts[16]);
    wheels_on = stof(parts[17]);
    taxi_in = stof(parts[18]);
    crs_arr_time = stoi(parts[19]);
    arr_time = stof(parts[20]);
    arr_delay = stof(parts[21]);
    canceled = static_cast<bool>(stoi(parts[22]));
    cancellation_code = parts[23].empty() ? '\0' : parts[23][0];
    diverted = static_cast<bool>(stoi(parts[24]));
    crs_elapsed = stof(parts[25]);
    actual_elapsed = stof(parts[26]);
    air_time = stof(parts[27]);
    distance = stof(parts[28]);
    carrier_delay = static_cast<bool>(stoi(parts[29]));
    weather_delay = static_cast<bool>(stoi(parts[30]));
    nas_delay = static_cast<bool>(stoi(parts[31]));
    security_delay = static_cast<bool>(stoi(parts[32]));
    late_aircraft_delay = static_cast<bool>(stoi(parts[33]));
}

void flight::print() {
    cout << "{"
         << "\"year\": " << year << ", "
         << "\"month\": " << month << ", "
         << "\"month_day\": " << month_day << ", "
         << "\"week_day\": " << week_day << ", "
         << "\"carrier_id\": " << '\"' << carrier_id << '\"' << ", "
         << "\"flight_number\": " << flight_number << ", "
         << "\"origin_code\": " << '\"' << origin_code << '\"' << ", "
         << "\"origin_city\": " << '\"' << origin_city << '\"' << ", "
         << "\"origin_state\": " << '\"' << origin_state << '\"' << ", "
         << "\"dest_code\": " << '\"' << dest_code << '\"' << ", "
         << "\"dest_city\": " << '\"' << dest_city << '\"' << ", "
         << "\"dest_state\": " << '\"' << dest_state << '\"' << ", "
         << "\"crs_dep_time\": " << crs_dep_time << ", "
         << "\"dep_time\": " << dep_time << ", "
         << "\"dep_delay\": " << dep_delay << ", "
         << "\"taxi_out\": " << taxi_out << ", "
         << "\"wheels_off\": " << wheels_off << ", "
         << "\"wheels_on\": " << wheels_on << ", "
         << "\"taxi_in\": " << taxi_in << ", "
         << "\"crs_arr_time\": " << crs_arr_time << ", "
         << "\"arr_time\": " << arr_time << ", "
         << "\"arr_delay\": " << arr_delay << ", "
         << "\"canceled\": " << canceled << ", "
         << "\"cancellation_code\": " << '\"' << cancellation_code << '\"' << ", "
         << "\"diverted\": " << diverted << ", "
         << "\"crs_elapsed\": " << crs_elapsed << ", "
         << "\"actual_elapsed\": " << actual_elapsed << ", "
         << "\"air_time\": " << air_time << ", "
         << "\"distance\": " << distance << ", "
         << "\"carrier_delay\": " << carrier_delay << ", "
         << "\"weather_delay\": " << weather_delay << ", "
         << "\"nas_delay\": " << nas_delay << ", "
         << "\"security_delay\": " << security_delay << ", "
         << "\"late_aircraft_delay\": " << late_aircraft_delay
         << "}\n";
}
