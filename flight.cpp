//
// Created by Дмитрий Егорычев
//

#include "flight.h"
#include <iostream>
#include <sstream>
#define read(x)

using namespace std;


flight::flight(const string &parts) {
    stringstream ss(parts);
    ss >> year >> month >> month_day >> week_day >> carrier_id >> flight_number
            >> origin_code >> origin_city >> origin_state >> dest_code >> dest_city
            >> dest_state >> crs_dep_time >> dep_time >> dep_delay >> taxi_out
            >> wheels_off >> wheels_on >> taxi_in >> crs_arr_time >> arr_time
            >> arr_delay >> canceled >> cancellation_code >> diverted
            >> crs_elapsed >> actual_elapsed >> air_time >> distance >> carrier_delay
            >> weather_delay >> nas_delay >> security_delay >> late_aircraft_delay;
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
