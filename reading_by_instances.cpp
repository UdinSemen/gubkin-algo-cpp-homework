#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <csv2/reader.hpp>
#include "flight.h"

using namespace std;

string parse_line(const string &line) {
    istringstream iss(line);
    string token;
    vector<string> fields;
    while (getline(iss, token, ';'))
        fields.push_back(token);
    if (!line.empty() && line.back() == ';')
        fields.emplace_back("");
    stringstream ss;
    bool first = true;
    for (const auto &f: fields) {
        if (!first) ss << ' ';
        first = false;
        if (f.empty()) ss << 0;
        else ss << f;
    }
    return ss.str();
}

vector<string> sep_line(const string &line) {
    istringstream iss(line);
    string token;
    vector<string> fields;
    while (getline(iss, token, ';')) {
        if (token.empty())
            fields.push_back("0");
        else
            fields.push_back(token);
    }
    if (!line.empty() && line.back() == ';')
        fields.emplace_back("");
    return fields;
}

int reading_by_instances() {
    ifstream fin("../flight_data_2024_semicolon.csv");
    if (!fin.is_open()) {
        cout << "File is unavailable to load" << endl;
        return 1;
    }
    vector<flight> flights;
    string line;
    time_t before_read = time(nullptr);
    getline(fin, line); // read header
    while (getline(fin, line)) {
        flight current_flight;
        current_flight.by_instances(parse_line(line));
        flights.emplace_back(current_flight);
    }
    time_t after_read = time(nullptr);
    cout << "Time taken to read and parse the file: " << difftime(after_read, before_read) << " seconds." << endl;
    return 0;
}

int reading_by_strings() {
    ifstream fin("../flight_data_2024_semicolon.csv");
    if (!fin.is_open()) {
        cout << "File is unavailable to load" << endl;
        return 1;
    }
    vector<flight> flights;
    string line;
    time_t before_read = time(nullptr);
    getline(fin, line); // read header
    while (getline(fin, line)) {
        flight current_flight;
        current_flight.by_slices(sep_line(line));
        flights.emplace_back(current_flight);
    }
    time_t after_read = time(nullptr);
    cout << "Time taken to read and parse the file: " << difftime(after_read, before_read) << " seconds." << endl;
    return 0;
}

int reading_by_library() {
    csv2::Reader<csv2::delimiter<';'>> csv;
    csv.mmap("../flight_data_2024_semicolon.csv");
    vector<flight> flights;
    time_t before_read = time(nullptr);
    for (const auto &row: csv) {
        if (row.length() == 0)
            continue;
        vector<string> parts;
        for (const auto cell: row) {
            string value;
            cell.read_value(value);
            if (value.empty())
                parts.emplace_back("0");
            else
                parts.emplace_back(value);
        }
        flight current_flight;
        current_flight.by_slices(parts);
        flights.emplace_back(current_flight);
    }
    time_t after_read = time(nullptr);
    cout << "Time taken to read and parse the file: " << difftime(after_read, before_read) << " seconds." << endl;
    return 0;
}

int main() {
    // return reading_by_instances();
    // return reading_by_strings();
    return reading_by_library();
}
