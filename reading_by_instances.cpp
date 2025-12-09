#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <unordered_set>
#include <csv2/reader.hpp>
#include "flight.h"

using namespace std;

string parse_line(const string& line) {
    istringstream iss(line);
    string token;
    vector<string> fields;
    while (getline(iss, token, ';'))
        fields.push_back(token);
    if (!line.empty() && line.back() == ';')
        fields.emplace_back("");
    stringstream ss;
    bool first = true;
    for (const auto& f : fields) {
        if (!first) ss << ' ';
        first = false;
        if (f.empty()) ss << 0;
        else ss << f;
    }
    return ss.str();
}

vector<string> sep_line(const string& line) {
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

void save_unique_keys_to_csv(const unordered_set<flight>& flights, const string& filename) {
    ofstream out_file(filename);
    if (!out_file.is_open()) {
        cout << "Cannot open file for writing: " << filename << endl;
        return;
    }

    out_file << "unique_key\n";

    for (const auto& f : flights) {
        out_file << f.get_unique_key() << "\n";
    }

    out_file.close();
    cout << "Saved " << flights.size() << " unique flight keys to " << filename << endl;
}

int reading_by_instances() {
    ifstream fin("../flight_data_2024_semicolon.csv");
    if (!fin.is_open()) {
        cout << "File is unavailable to load" << endl;
        return 1;
    }

    unordered_set<flight> unique_flights;
    string line;
    time_t before_read = time(nullptr);

    getline(fin, line);

    while (getline(fin, line)) {
        flight current_flight;
        current_flight.by_instances(parse_line(line));
        unique_flights.insert(current_flight);
    }

    time_t after_read = time(nullptr);
    cout << "Time taken to read and parse the file: " << difftime(after_read, before_read) << " seconds." << endl;
    cout << "Unique flights found: " << unique_flights.size() << endl;

    save_unique_keys_to_csv(unique_flights, "../unique_by_instances.csv");

    return 0;
}

int reading_by_strings() {
    ifstream fin("../flight_data_2024_semicolon.csv");
    if (!fin.is_open()) {
        cout << "File is unavailable to load" << endl;
        return 1;
    }

    unordered_set<flight> unique_flights;
    string line;
    time_t before_read = time(nullptr);

    getline(fin, line);

    while (getline(fin, line)) {
        flight current_flight;
        current_flight.by_slices(sep_line(line));
        unique_flights.insert(current_flight);
    }

    time_t after_read = time(nullptr);
    cout << "Time taken to read and parse the file: " << difftime(after_read, before_read) << " seconds." << endl;
    cout << "Unique flights found: " << unique_flights.size() << endl;

    save_unique_keys_to_csv(unique_flights, "../unique_by_strings.csv");

    return 0;
}

int reading_by_library() {
    csv2::Reader<csv2::delimiter<';'>> csv;
    if (!csv.mmap("../flight_data_2024_semicolon.csv")) {
        cout << "File is unavailable to load" << endl;
        return 1;
    }

    unordered_set<flight> unique_flights;
    time_t before_read = time(nullptr);

    for (const auto& row : csv) {
        if (row.length() == 0)
            continue;

        vector<string> parts;
        for (const auto cell : row) {
            string value;
            cell.read_value(value);
            if (value.empty())
                parts.emplace_back("0");
            else
                parts.emplace_back(value);
        }

        flight current_flight;
        current_flight.by_slices(parts);
        unique_flights.insert(current_flight);
    }

    time_t after_read = time(nullptr);
    cout << "Time taken to read and parse the file: " << difftime(after_read, before_read) << " seconds." << endl;
    cout << "Unique flights found: " << unique_flights.size() << endl;

    save_unique_keys_to_csv(unique_flights, "../unique_by_library.csv");

    return 0;
}

int main() {

    cout << "Testing reading_by_instances():" << endl;
    reading_by_instances();

    cout << "\nTesting reading_by_strings():" << endl;
    reading_by_strings();

    cout << "\nTesting reading_by_library():" << endl;
    reading_by_library();

    return 0;
}