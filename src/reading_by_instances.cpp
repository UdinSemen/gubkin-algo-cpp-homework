#include "reading_by_instances.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

// Опционально: если есть библиотека csv2
 #include <csv2/reader.hpp>

using namespace std;

// ============================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================

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
        fields.emplace_back("0");
    return fields;
}

// ============================================
// ФУНКЦИЯ СОХРАНЕНИЯ
// ============================================

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

// ============================================
// ФУНКЦИИ ЧТЕНИЯ (возвращают данные)
// ============================================

size_t get_file_size(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) return 0;
    return file.tellg();
}

unordered_set<flight> read_flights_by_instances(const string& filename, bool show_progress, size_t max_lines) {
    unordered_set<flight> unique_flights;

    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "File is unavailable to load: " << filename << endl;
        return unique_flights;
    }

    size_t file_size = get_file_size(filename);
    size_t bytes_read = 0;
    size_t line_count = 0;
    int last_percent = -1;

    string line;
    getline(fin, line); // Skip header
    bytes_read += line.length() + 1;

    while (getline(fin, line)) {
        // Проверка ограничения на количество строк
        if (max_lines > 0 && line_count >= max_lines) {
            break;
        }

        flight current_flight;
        current_flight.by_instances(parse_line(line));
        unique_flights.insert(current_flight);

        bytes_read += line.length() + 1;
        line_count++;

        if (show_progress && file_size > 0) {
            int percent = (bytes_read * 100) / file_size;
            if (percent != last_percent && percent % 5 == 0) {
                cout << "\r  Loading: " << percent << "% | Lines: " << line_count
                     << " | Unique: " << unique_flights.size() << flush;
                last_percent = percent;
            }
        }
    }

    if (show_progress) {
        if (max_lines > 0 && line_count >= max_lines) {
            cout << "\r  Loading: | Lines: " << line_count
                 << " | Unique: " << unique_flights.size() << " (max: " << max_lines << ")     " << endl;
        } else {
            cout << "\r  Loading: 100% | Lines: " << line_count
                 << " | Unique: " << unique_flights.size() << "     " << endl;
        }
    }

    return unique_flights;
}

unordered_set<flight> read_flights_by_strings(const string& filename, bool show_progress, size_t max_lines) {
    unordered_set<flight> unique_flights;

    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "File is unavailable to load: " << filename << endl;
        return unique_flights;
    }

    size_t file_size = get_file_size(filename);
    size_t bytes_read = 0;
    size_t line_count = 0;
    int last_percent = -1;

    string line;
    getline(fin, line); // Skip header
    bytes_read += line.length() + 1;

    while (getline(fin, line)) {
        // Проверка ограничения на количество строк
        if (max_lines > 0 && line_count >= max_lines) {
            break;
        }

        flight current_flight;
        current_flight.by_slices(sep_line(line));
        unique_flights.insert(current_flight);

        bytes_read += line.length() + 1;
        line_count++;

        if (show_progress && file_size > 0) {
            int percent = (bytes_read * 100) / file_size;
            if (percent != last_percent && percent % 5 == 0) {
                cout << "\r  Loading: " << percent << "% | Lines: " << line_count
                     << " | Unique: " << unique_flights.size() << flush;
                last_percent = percent;
            }
        }
    }

    if (show_progress) {
        if (max_lines > 0 && line_count >= max_lines) {
            cout << "\r  Loading: | Lines: " << line_count
                 << " | Unique: " << unique_flights.size() << " (max: " << max_lines << ")     " << endl;
        } else {
            cout << "\r  Loading: 100% | Lines: " << line_count
                 << " | Unique: " << unique_flights.size() << "     " << endl;
        }
    }

    return unique_flights;
}

// Версия с библиотекой csv2 (закомментирована, если нет библиотеки)
unordered_set<flight> read_flights_by_library(const string& filename, bool show_progress, size_t max_lines) {
    unordered_set<flight> unique_flights;

    csv2::Reader<csv2::delimiter<';'>> csv;
    if (!csv.mmap(filename)) {
        cerr << "File is unavailable to load: " << filename << endl;
        return unique_flights;
    }

    size_t row_count = 0;
    size_t last_update = 0;

    for (const auto& row : csv) {
        if (row.length() == 0)
            continue;

        // Проверка ограничения на количество строк
        if (max_lines > 0 && row_count >= max_lines) {
            break;
        }

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

        row_count++;
        if (show_progress && (row_count - last_update) >= 100000) {
            cout << "\r  Loading: Lines: " << row_count
                 << " | Unique: " << unique_flights.size() << flush;
            last_update = row_count;
        }
    }

    if (show_progress) {
        if (max_lines > 0 && row_count >= max_lines) {
            cout << "\r  Loading: | Lines: " << row_count
                 << " | Unique: " << unique_flights.size() << " (max: " << max_lines << ")     " << endl;
        } else {
            cout << "\r  Loading: 100% | Lines: " << row_count
                 << " | Unique: " << unique_flights.size() << "     " << endl;
        }
    }

    return unique_flights;
}

