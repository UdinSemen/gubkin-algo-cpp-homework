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
    string result;
    result.reserve(line.size());
    bool field_empty = true;
    for (char c: line) {
        if (c == ';') {
            if (!result.empty())
                result.push_back(' ');
            if (field_empty)
                result.push_back('0');
            field_empty = true;
        } else {
            result.push_back(c);
            field_empty = false;
        }
    }
    if (!result.empty())
        result.push_back(' ');
    if (field_empty)
        result.push_back('0');
    return result;
}

vector<string> sep_line(const string &line) {
    vector<string> fields;
    fields.reserve(16);
    size_t start = 0;
    const size_t n = line.size();
    for (size_t i = 0; i <= n; ++i) {
        if (i == n || line[i] == ';') {
            if (i == start)
                fields.emplace_back("0");
            else
                fields.emplace_back(line.substr(start, i - start));
            start = i + 1;
        }
    }
    return fields;
}

int reading_by_instances() {
    ifstream fin("../flight_data_2024_semicolon.csv");
    if (!fin.is_open()) {
        cout << "File is unavailable to load" << endl;
        return 1;
    }
    vector<flight> flights;
    flights.reserve(200000);
    string line;
    auto before_read = std::chrono::steady_clock::now();
    getline(fin, line); // read header
    while (getline(fin, line)) {
        flight current_flight;
        current_flight.by_instances(parse_line(line));
        flights.emplace_back(std::move(current_flight));
    }
    auto after_read = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = after_read - before_read;
    cout << "Time taken to read and parse the file: "
            << elapsed.count() << " seconds." << endl;
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
    auto before_read = std::chrono::steady_clock::now();
    getline(fin, line); // read header
    while (getline(fin, line)) {
        flight current_flight;
        current_flight.by_slices(sep_line(line));
        flights.emplace_back(current_flight);
    }
    auto after_read = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = after_read - before_read;
    cout << "Time taken to read and parse the file: "
            << elapsed.count() << " seconds." << endl;
    return 0;
}

int reading_by_library() {
    csv2::Reader<csv2::delimiter<';'> > csv;
    csv.mmap("../flight_data_2024_semicolon.csv");
    vector<flight> flights;
    auto before_read = std::chrono::steady_clock::now();
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
    auto after_read = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = after_read - before_read;
    cout << "Time taken to read and parse the file: "
            << elapsed.count() << " seconds." << endl;
    return 0;
}

int main() {
    // return reading_by_instances();
    // return reading_by_strings();
    return reading_by_library();
}
