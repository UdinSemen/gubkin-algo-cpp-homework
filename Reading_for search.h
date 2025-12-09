#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "flight.h"
using namespace std;

vector<flight> reading_by_instances() {
    ifstream fin("../flight_data_2024_semicolon.csv");
    if (!fin.is_open()) {
        cout << "File is unavailable to load" << endl;
        return 1;
    }
    vector<flight> flights;
    string line;
    getline(fin, line); // read header
    while (getline(fin, line)) {
        flight current_flight;
        current_flight.by_instances(parse_line(line));
        flights.emplace_back(current_flight);
    }
    return flights;
}

