#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <limits>
#include <cstring>
#include <cstdio>

#include "flight.h"
#include "flight_organizer.h"
#include "reading_by_instances.h"
#include "compression.h"
#include "sorting.h"
#include "Search_Algs.h"
#include "graph.h"

using namespace std;
using namespace std::chrono;

string format_bytes(size_t bytes) {
    if (bytes < 1024) return to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return to_string(bytes / 1024) + " KB";
    return to_string(bytes / (1024 * 1024)) + " MB";
}

void test_decompression(const string &original_file, const string &compressed_file) {
    cout << "\n=== ТЕСТИРОВАНИЕ РАСПАКОВКИ ===" << endl;

    string decompressed_file = original_file + ".decompressed";

    // Удаляем старый файл если есть
    ifstream check(decompressed_file);
    if (check.good()) {
        check.close();
        remove(decompressed_file.c_str());
    }

    // Распаковка
    auto start = steady_clock::now();
    decompressFileStreaming(compressed_file, decompressed_file);
    auto end = steady_clock::now();
    double decompress_time = duration_cast<milliseconds>(end - start).count() / 1000.0;

    // Проверка размера
    ifstream orig(original_file, ios::binary | ios::ate);
    ifstream decomp(decompressed_file, ios::binary | ios::ate);

    if (!orig.is_open() || !decomp.is_open()) {
        cout << "Ошибка открытия файлов для проверки" << endl;
        return;
    }

    size_t orig_size = orig.tellg();
    size_t decomp_size = decomp.tellg();

    cout << "Время распаковки: " << fixed << setprecision(3) << decompress_time << " сек" << endl;
    cout << "Размер оригинала: " << format_bytes(orig_size) << endl;
    cout << "Размер распакованного: " << format_bytes(decomp_size) << endl;

    // Проверка содержимого
    if (orig_size != decomp_size) {
        cout << "Результат: ОШИБКА - размеры не совпадают" << endl;
        orig.close();
        decomp.close();
        return;
    }

    // Побайтовое сравнение
    orig.seekg(0);
    decomp.seekg(0);

    const size_t BUFFER_SIZE = 1024 * 1024;
    vector<char> buf1(BUFFER_SIZE);
    vector<char> buf2(BUFFER_SIZE);
    bool identical = true;

    while (identical && orig.read(buf1.data(), BUFFER_SIZE) && decomp.read(buf2.data(), BUFFER_SIZE)) {
        size_t bytes = orig.gcount();
        if (bytes != decomp.gcount() || memcmp(buf1.data(), buf2.data(), bytes) != 0) {
            identical = false;
            break;
        }
    }

    orig.close();
    decomp.close();
}

void compare_storage_types(const vector<flight> &all_flights) {
    cout << "\n=== СРАВНЕНИЕ ТИПОВ ХРАНЕНИЯ ===" << endl;
    cout << "Размер данных: " << all_flights.size() << " записей" << endl << endl;

    // Ограничим размер для тестирования, если данных слишком много
    // size_t test_size = min(all_flights.size(), size_t(50000));
    vector<flight> test_data(all_flights.begin(), all_flights.end());

    // cout << "Размер тестового набора: " << test_size << " записей" << endl << endl;

    FlightOrganizer organizer;

    struct StorageResult {
        string name;
        double insert_time;
        double search_time;
        size_t memory_estimate;
    };
    vector<StorageResult> results;

    // 1. Vector
    cout << "1. std::vector<flight>" << endl; {
        vector<flight> container;
        auto start = steady_clock::now();
        for (const auto &f: test_data) {
            container.push_back(f);
        }
        auto end = steady_clock::now();
        double insert_time = duration_cast<milliseconds>(end - start).count() / 1000.0;

        // Поиск первого элемента
        string search_key = test_data[0].get_unique_key();
        start = steady_clock::now();
        for (int i = 0; i < 1000; ++i) {
            for (const auto &f: container) {
                if (f.get_unique_key() == search_key) break;
            }
        }
        end = steady_clock::now();
        double search_time = duration_cast<microseconds>(end - start).count() / 1000000.0;

        size_t mem = container.capacity() * sizeof(flight);
        results.push_back({"vector", insert_time, search_time, mem});

        cout << "  Время вставки: " << fixed << setprecision(3) << insert_time << " сек" << endl;
        cout << "  Время поиска (1000 операций): " << fixed << setprecision(6) << search_time << " сек" << endl;
        cout << "  Память: ~" << format_bytes(mem) << endl;
    }

    // 2. Unordered_set
    cout << "\n2. std::unordered_set<flight>" << endl; {
        unordered_set<flight> container;
        auto start = steady_clock::now();
        for (const auto &f: test_data) {
            container.insert(f);
        }
        auto end = steady_clock::now();
        double insert_time = duration_cast<milliseconds>(end - start).count() / 1000.0;

        string search_key = test_data[0].get_unique_key();
        start = steady_clock::now();
        for (int i = 0; i < 1000; ++i) {
            for (const auto &f: container) {
                if (f.get_unique_key() == search_key) break;
            }
        }
        end = steady_clock::now();
        double search_time = duration_cast<microseconds>(end - start).count() / 1000000.0;

        size_t mem = container.size() * sizeof(flight) + container.bucket_count() * sizeof(void *);
        results.push_back({"unordered_set", insert_time, search_time, mem});

        cout << "  Время вставки: " << fixed << setprecision(3) << insert_time << " сек" << endl;
        cout << "  Время поиска (1000 операций): " << fixed << setprecision(6) << search_time << " сек" << endl;
        cout << "  Память: ~" << format_bytes(mem) << endl;
    }

    // 3. Set
    cout << "\n3. std::set<flight>" << endl; {
        set<flight> container;
        auto start = steady_clock::now();
        for (const auto &f: test_data) {
            container.insert(f);
        }
        auto end = steady_clock::now();
        double insert_time = duration_cast<milliseconds>(end - start).count() / 1000.0;

        string search_key = test_data[0].get_unique_key();
        start = steady_clock::now();
        for (int i = 0; i < 1000; ++i) {
            for (const auto &f: container) {
                if (f.get_unique_key() == search_key) break;
            }
        }
        end = steady_clock::now();
        double search_time = duration_cast<microseconds>(end - start).count() / 1000000.0;

        size_t mem = container.size() * (sizeof(flight) + sizeof(void *));
        results.push_back({"set", insert_time, search_time, mem});

        cout << "  Время вставки: " << fixed << setprecision(3) << insert_time << " сек" << endl;
        cout << "  Время поиска: " << fixed << setprecision(6) << search_time << " сек" << endl;
        cout << "  Память: ~" << format_bytes(mem) << endl;
    }

    // 4. Unordered_map
    cout << "\n4. std::unordered_map<string, flight>" << endl; {
        unordered_map<string, flight> container;
        auto start = steady_clock::now();
        for (const auto &f: test_data) {
            container[f.get_unique_key()] = f;
        }
        auto end = steady_clock::now();
        double insert_time = duration_cast<milliseconds>(end - start).count() / 1000.0;

        string search_key = test_data[0].get_unique_key();
        start = steady_clock::now();
        for (int i = 0; i < 1000; ++i) {
            auto it = container.find(search_key);
        }
        end = steady_clock::now();
        double search_time = duration_cast<microseconds>(end - start).count() / 1000000.0;

        size_t mem = container.size() * (sizeof(string) + sizeof(flight)) + container.bucket_count() * sizeof(void *);
        results.push_back({"unordered_map", insert_time, search_time, mem});

        cout << "  Время вставки: " << fixed << setprecision(3) << insert_time << " сек" << endl;
        cout << "  Время поиска: " << fixed << setprecision(6) << search_time << " сек" << endl;
        cout << "  Память: ~" << format_bytes(mem) << endl;
    }

    // 5. Map
    cout << "\n5. std::map<string, flight>" << endl; {
        map<string, flight> container;
        auto start = steady_clock::now();
        for (const auto &f: test_data) {
            container[f.get_unique_key()] = f;
        }
        auto end = steady_clock::now();
        double insert_time = duration_cast<milliseconds>(end - start).count() / 1000.0;

        string search_key = test_data[0].get_unique_key();
        start = steady_clock::now();
        for (int i = 0; i < 1000; ++i) {
            auto it = container.find(search_key);
        }
        end = steady_clock::now();
        double search_time = duration_cast<microseconds>(end - start).count() / 1000000.0;

        size_t mem = container.size() * (sizeof(string) + sizeof(flight) + 3 * sizeof(void *));
        results.push_back({"map", insert_time, search_time, mem});

        cout << "  Время вставки: " << fixed << setprecision(3) << insert_time << " сек" << endl;
        cout << "  Время поиска: " << fixed << setprecision(6) << search_time << " сек" << endl;
        cout << "  Память: ~" << format_bytes(mem) << endl;
    }

    // 6. Unordered_multimap
    cout << "\n6. std::unordered_multimap<string, flight>" << endl; {
        unordered_multimap<string, flight> container;
        auto start = steady_clock::now();
        for (const auto &f: test_data) {
            container.insert({f.get_unique_key(), f});
        }
        auto end = steady_clock::now();
        double insert_time = duration_cast<milliseconds>(end - start).count() / 1000.0;

        string search_key = test_data[0].get_unique_key();
        start = steady_clock::now();
        for (int i = 0; i < 1000; ++i) {
            auto range = container.equal_range(search_key);
        }
        end = steady_clock::now();
        double search_time = duration_cast<microseconds>(end - start).count() / 1000000.0;

        size_t mem = container.size() * (sizeof(string) + sizeof(flight)) + container.bucket_count() * sizeof(void *);
        results.push_back({"unordered_multimap", insert_time, search_time, mem});

        cout << "  Время вставки: " << fixed << setprecision(3) << insert_time << " сек" << endl;
        cout << "  Время поиска: " << fixed << setprecision(6) << search_time << " сек" << endl;
        cout << "  Память: ~" << format_bytes(mem) << endl;
    }

    // 7. Multimap
    cout << "\n7. std::multimap<string, flight>" << endl; {
        multimap<string, flight> container;
        auto start = steady_clock::now();
        for (const auto &f: test_data) {
            container.insert({f.get_unique_key(), f});
        }
        auto end = steady_clock::now();
        double insert_time = duration_cast<milliseconds>(end - start).count() / 1000.0;

        string search_key = test_data[0].get_unique_key();
        start = steady_clock::now();
        for (int i = 0; i < 1000; ++i) {
            auto range = container.equal_range(search_key);
        }
        end = steady_clock::now();
        double search_time = duration_cast<microseconds>(end - start).count() / 1000000.0;

        size_t mem = container.size() * (sizeof(string) + sizeof(flight) + 3 * sizeof(void *));
        results.push_back({"multimap", insert_time, search_time, mem});

        cout << "  Время вставки: " << fixed << setprecision(3) << insert_time << " сек" << endl;
        cout << "  Время поиска: " << fixed << setprecision(6) << search_time << " сек" << endl;
        cout << "  Память: ~" << format_bytes(mem) << endl;
    }

    // Сравнительная таблица
    cout << "\n--- Сравнительная таблица ---" << endl;
    cout << "\t" << left << "Контейнер"
            << "\t" << "Вставка (сек)"
            << "\t" << "Поиск (сек)"
            << "\t" << "Память" << endl;
    cout << string(73, '-') << endl;

    for (const auto &r: results) {
        cout << setw(22) << left << r.name
                << setw(18) << fixed << setprecision(3) << r.insert_time
                << setw(18) << setprecision(6) << r.search_time
                << format_bytes(r.memory_estimate) << endl;
    }
}

void compare_reading_methods(const string &csv_file, size_t max_lines = 0) {
    cout << "\n=== СРАВНЕНИЕ МЕТОДОВ ЧТЕНИЯ CSV ===" << endl;

    size_t file_size = get_file_size(csv_file);
    cout << "Размер файла: " << format_bytes(file_size) << endl;
    if (max_lines > 0) {
        cout << "Используется выборка: первые " << max_lines << " строк" << endl;
    }
    cout << endl;

    struct ReadingResult {
        string method_name;
        double time_seconds;
        size_t unique_count;
        bool success;
    };

    vector<ReadingResult> results;

    // Метод 1: by_instances
    cout << "Метод 1: by_instances (parse_line + by_instances)" << endl; {
        auto start = steady_clock::now();
        auto flights = read_flights_by_instances(csv_file, true, max_lines);
        auto end = steady_clock::now();
        double elapsed = duration_cast<milliseconds>(end - start).count() / 1000.0;

        results.push_back({"by_instances", elapsed, flights.size(), !flights.empty()});
        cout << "  Время: " << fixed << setprecision(3) << elapsed << " сек" << endl;
        cout << "  Записей: " << flights.size() << endl;
    }

    // Метод 2: by_strings
    cout << "\nМетод 2: by_strings (sep_line + by_slices)" << endl; {
        auto start = steady_clock::now();
        auto flights = read_flights_by_strings(csv_file, true, max_lines);
        auto end = steady_clock::now();
        double elapsed = duration_cast<milliseconds>(end - start).count() / 1000.0;

        results.push_back({"by_strings", elapsed, flights.size(), !flights.empty()});
        cout << "  Время: " << fixed << setprecision(3) << elapsed << " сек" << endl;
        cout << "  Записей: " << flights.size() << endl;
    }

    // Метод 3: by_library
    cout << "\nМетод 3: by_library (csv2)" << endl;
    try {
        auto start = steady_clock::now();
        auto flights = read_flights_by_library(csv_file, true, max_lines);
        auto end = steady_clock::now();
        double elapsed = duration_cast<milliseconds>(end - start).count() / 1000.0;

        results.push_back({"by_library", elapsed, flights.size(), !flights.empty()});
        cout << "  Время: " << fixed << setprecision(3) << elapsed << " сек" << endl;
        cout << "  Записей: " << flights.size() << endl;
    } catch (const exception &e) {
        cout << "  Недоступно: " << e.what() << endl;
        results.push_back({"by_library", 0, 0, false});
    }

    // Сравнительная таблица
    cout << "\n--- Результаты сравнения ---" << endl;
    cout << setw(20) << left << "Метод"
            << setw(15) << "Время (сек)"
            << setw(15) << "Записей"
            << setw(15) << "Относит." << endl;
    cout << string(65, '-') << endl;

    double fastest_time = numeric_limits<double>::max();
    for (const auto &r: results) {
        if (r.success && r.time_seconds > 0 && r.time_seconds < fastest_time) {
            fastest_time = r.time_seconds;
        }
    }

    for (const auto &r: results) {
        cout << setw(20) << left << r.method_name;

        if (r.success) {
            cout << setw(15) << fixed << setprecision(3) << r.time_seconds
                    << setw(15) << r.unique_count;

            if (r.time_seconds > 0 && fastest_time > 0) {
                double relative_speed = fastest_time / r.time_seconds;
                if (relative_speed >= 0.99) {
                    cout << "САМЫЙ БЫСТРЫЙ";
                } else {
                    cout << fixed << setprecision(1) << (relative_speed * 100) << "%";
                }
            }
        } else {
            cout << "НЕДОСТУПЕН";
        }
        cout << endl;
    }

    if (!results.empty()) {
        auto fastest = min_element(results.begin(), results.end(),
                                   [](const ReadingResult &a, const ReadingResult &b) {
                                       if (!a.success) return false;
                                       if (!b.success) return true;
                                       return a.time_seconds < b.time_seconds;
                                   });
    }
}

void compare_sorting_algorithms(const vector<flight> &test_data) {
    cout << "\n=== СРАВНЕНИЕ АЛГОРИТМОВ СОРТИРОВКИ ===" << endl;
    cout << "Размер выборки: " << test_data.size() << " записей" << endl;
    cout << "Критерий: сортировка по задержке прибытия (arrival delay)" << endl;

    struct SortResult {
        string name;
        double time_seconds;
        bool success;
    };
    vector<SortResult> results;

    // 1. mergeSortByArrivalDelay - рекурсивная merge sort
    cout << "1. mergeSortByArrivalDelay (рекурсивная Merge Sort)" << endl; {
        vector<flight> data_copy = test_data;
        auto start = steady_clock::now();
        mergeSortByArrivalDelay(data_copy);
        auto end = steady_clock::now();
        double elapsed = duration_cast<milliseconds>(end - start).count() / 1000.0;

        results.push_back({"mergeSortByArrivalDelay", elapsed, true});
        cout << "  Время: " << fixed << setprecision(3) << elapsed << " сек" << endl;

        cout << "  Первые 3: ";
        for (size_t i = 0; i < min(size_t(3), data_copy.size()); ++i) {
            if (i > 0) cout << ", ";
            cout << data_copy[i].get_arr_delay() << " мин";
        }
        cout << endl;
    }

    // 2. specialFlightSort - сортировка с специальным компаратором (std::sort)
    cout << "\n2. specialFlightSort (std::sort с компаратором: отмена + штат + задержка)" << endl; {
        vector<flight> data_copy = test_data;
        auto start = steady_clock::now();
        specialFlightSort(data_copy);
        auto end = steady_clock::now();
        double elapsed = duration_cast<milliseconds>(end - start).count() / 1000.0;

        results.push_back({"specialFlightSort", elapsed, true});
        cout << "  Время: " << fixed << setprecision(3) << elapsed << " сек" << endl;

        cout << "  Первые 3: ";
        for (size_t i = 0; i < min(size_t(3), data_copy.size()); ++i) {
            if (i > 0) cout << ", ";
            cout << "отм:" << (data_copy[i].is_canceled() ? "да" : "нет")
                    << " зад:" << data_copy[i].get_arr_delay() << "мин";
        }
        cout << endl;
    }

    // Результаты
    cout << "\n--- Результаты сравнения ---" << endl;
    cout << setw(45) << left << "Алгоритм"
            << setw(15) << "Время (сек)"
            << setw(20) << "Относительная скорость" << endl;
    cout << string(80, '-') << endl;

    // Находим самый быстрый
    double fastest_time = numeric_limits<double>::max();
    string fastest_name;
    for (const auto &r: results) {
        if (r.success && r.time_seconds < fastest_time) {
            fastest_time = r.time_seconds;
            fastest_name = r.name;
        }
    }

    for (const auto &r: results) {
        cout << setw(45) << left << r.name
                << setw(15) << fixed << setprecision(3) << r.time_seconds;

        if (r.name == fastest_name) {
            cout << "★ САМЫЙ БЫСТРЫЙ";
        } else {
            double ratio = r.time_seconds / fastest_time;
            cout << fixed << setprecision(2) << ratio << "x медленнее";
        }
        cout << endl;
    }
}

void compare_search_algorithms(const vector<flight> &all_flights) {
    cout << "\n=== СРАВНЕНИЕ АЛГОРИТМОВ ПОИСКА ===" << endl;
    cout << "Размер данных: " << all_flights.size() << " записей" << endl;
    cout << "Критерий поиска: дистанция = 1000 миль (для всех алгоритмов)" << endl << endl;

    struct SearchResult {
        string name;
        double time_seconds;
        double sort_time;
        size_t results_count;
        vector<flight> found_flights;
    };
    vector<SearchResult> results;

    float search_distance = 1000.0f;

    // Linear Search
    cout << "1. Линейный поиск" << endl; {
        auto start = steady_clock::now();
        auto search_results = linear_search(all_flights,
                                            [](const flight &f) { return f.getDistance(); },
                                            search_distance);
        auto end = steady_clock::now();
        double elapsed = duration_cast<milliseconds>(end - start).count() / 1000.0;
        results.push_back({"Linear Search", elapsed, 0.0, search_results.size(), search_results});
        cout << "  Найдено: " << search_results.size() << endl;

        // Вывод первых 5
        cout << "  Первые 5 найденных:" << endl;
        for (size_t i = 0; i < min(size_t(5), search_results.size()); ++i) {
            cout << "    " << (i + 1) << ". " << search_results[i].get_unique_key()
                    << " (дистанция: " << search_results[i].getDistance() << " миль)" << endl;
        }
    }

    // Binary Search (используем std::sort)
    cout << "\n2. Бинарный поиск (с std::sort)" << endl; {
        vector<flight> sorted_flights = all_flights;

        auto sort_start = steady_clock::now();
        std::sort(sorted_flights.begin(), sorted_flights.end(), [](const flight &a, const flight &b) {
            return a.getDistance() < b.getDistance();
        });
        auto sort_end = steady_clock::now();
        double sort_time = duration_cast<milliseconds>(sort_end - sort_start).count() / 1000.0;

        auto search_start = steady_clock::now();
        auto search_results = binary_search(sorted_flights,
                                            [](const flight &f) { return f.getDistance(); },
                                            search_distance);
        auto search_end = steady_clock::now();
        double search_time = duration_cast<milliseconds>(search_end - search_start).count() / 1000.0;
        double total_time = sort_time + search_time;

        results.push_back({"Binary Search", total_time, sort_time, search_results.size(), search_results});
        cout << "  Время сортировки (std::sort): " << fixed << setprecision(3) << sort_time << " сек" << endl;
        cout << "  Общее время: " << fixed << setprecision(3) << total_time << " сек" << endl;
        cout << "  Найдено: " << search_results.size() << endl;

        // Вывод первых 5
        cout << "  Первые 5 найденных:" << endl;
        for (size_t i = 0; i < min(size_t(5), search_results.size()); ++i) {
            cout << "    " << (i + 1) << ". " << search_results[i].get_unique_key()
                    << " (дистанция: " << search_results[i].getDistance() << " миль)" << endl;
        }
    }

    // Fibonacci Search (используем std::sort)
    cout << "\n3. Fibonacci поиск (с std::sort)" << endl; {
        vector<flight> sorted_flights = all_flights;

        auto sort_start = steady_clock::now();
        std::sort(sorted_flights.begin(), sorted_flights.end(), [](const flight &a, const flight &b) {
            return a.getDistance() < b.getDistance();
        });
        auto sort_end = steady_clock::now();
        double sort_time = duration_cast<milliseconds>(sort_end - sort_start).count() / 1000.0;

        auto search_start = steady_clock::now();
        auto search_results = Fibonacci_search(sorted_flights,
                                               [](const flight &f) { return f.getDistance(); },
                                               search_distance);
        auto search_end = steady_clock::now();
        double search_time = duration_cast<milliseconds>(search_end - search_start).count() / 1000.0;
        double total_time = sort_time + search_time;

        results.push_back({"Fibonacci Search", total_time, sort_time, search_results.size(), search_results});
        cout << "  Время сортировки (std::sort): " << fixed << setprecision(3) << sort_time << " сек" << endl;
        cout << "  Общее время: " << fixed << setprecision(3) << total_time << " сек" << endl;
        cout << "  Найдено: " << search_results.size() << endl;

        // Вывод первых 5
        cout << "  Первые 5 найденных:" << endl;
        for (size_t i = 0; i < min(size_t(5), search_results.size()); ++i) {
            cout << "    " << (i + 1) << ". " << search_results[i].get_unique_key()
                    << " (дистанция: " << search_results[i].getDistance() << " миль)" << endl;
        }
    }

    cout << "\n--- Результаты сравнения ---" << endl;
    cout << setw(25) << left << "Алгоритм"
            << setw(18) << "Время поиска"
            << setw(15) << "Найдено" << endl;
    cout << string(76, '-') << endl;
    for (const auto &r: results) {
        cout << setw(25) << left << r.name
                << setw(18) << fixed << setprecision(3) << (r.time_seconds)
                << r.results_count << endl;
    }
}

int main() {
    auto program_start = steady_clock::now();

    const string CSV_FILE = "../data/flight_data_2024_semicolon.csv";
    // const string CSV_FILE = "../data/small.csv";

    // Размер выборки для ТЕСТИРОВАНИЯ сортировки и поиска
    // (загрузка данных будет БЕЗ ограничения)
    const size_t TEST_SAMPLE_SIZE = 100000;

    // Сравнение методов чтения (на выборке для ускорения)
    compare_reading_methods(CSV_FILE, TEST_SAMPLE_SIZE);

    // Загрузка данных с прогрессом (загружаем ВСЕ данные для работы программы)
    cout << "\n=== ЗАГРУЗКА ДАННЫХ (полная) ===" << endl;
    cout << "Загружается весь файл..." << endl;

    auto load_start = steady_clock::now();
    auto flights = read_flights_by_instances(CSV_FILE, true, 0); // 0 = без ограничения
    auto load_end = steady_clock::now();
    double load_time = duration_cast<milliseconds>(load_end - load_start).count() / 1000.0;

    if (flights.empty()) {
        cerr << "Ошибка: не удалось загрузить данные" << endl;
        return 1;
    }

    cout << "Загружено записей: " << flights.size() << endl;
    cout << "Время загрузки: " << fixed << setprecision(2) << load_time << " сек" << endl;

    vector<flight> all_flights(flights.begin(), flights.end());

    // Создание тестовой выборки для сортировки и поиска
    size_t sample_size = min(TEST_SAMPLE_SIZE, all_flights.size());
    vector<flight> test_sample(all_flights.begin(), all_flights.begin() + sample_size);

    cout << "\n=== НАСТРОЙКИ ТЕСТИРОВАНИЯ ===" << endl;
    cout << "Общий размер данных: " << all_flights.size() << " записей" << endl;
    cout << "Размер тестовой выборки (для сортировки/поиска): " << test_sample.size() << " записей" << endl;

    // Сравнение типов хранения (на тестовой выборке)
    compare_storage_types(test_sample);

    // Сжатие ВСЕГО датасета
    cout << "\n=== СЖАТИЕ ДАННЫХ (весь файл) ===" << endl;
    string compressed_file = CSV_FILE + ".lzss";

    auto compress_start = steady_clock::now();
    compressFileStreaming(CSV_FILE, compressed_file);
    auto compress_end = steady_clock::now();
    double compress_time = duration_cast<milliseconds>(compress_end - compress_start).count() / 1000.0;

    size_t original_size = get_file_size(CSV_FILE);
    ifstream comp_check(compressed_file, ios::binary | ios::ate);
    if (comp_check.is_open()) {
        size_t comp_size = comp_check.tellg();
        cout << "Исходный размер: " << format_bytes(original_size) << endl;
        cout << "Сжатый размер: " << format_bytes(comp_size) << endl;
        cout << "Степень сжатия: " << fixed << setprecision(1)
                << (100.0 * (1.0 - (double) comp_size / original_size)) << "%" << endl;
        cout << "Время сжатия: " << fixed << setprecision(2) << compress_time << " сек" << endl;
        comp_check.close();
    }

    // Тестирование распаковки ВСЕГО датасета
    test_decompression(CSV_FILE, compressed_file);

    // Сравнение алгоритмов сортировки (на тестовой выборке)
    compare_sorting_algorithms(test_sample);

    // Сравнение алгоритмов поиска (на тестовой выборке)
    compare_search_algorithms(test_sample);

    // Граф и алгоритм Дейкстры
    cout << "\n=== ПОСТРОЕНИЕ ГРАФА ГОРОДОВ ===" << endl;
    Graph cityGraph;
    map<pair<string, string>, double> cityConnections;

    for (const auto &f: all_flights) {
        string origin = f.getOriginCity();
        string dest = f.getDestCity();
        double distance = f.getDistance();

        if (!origin.empty() && !dest.empty() && distance > 0) {
            auto key = make_pair(origin, dest);
            if (cityConnections.find(key) == cityConnections.end()) {
                cityConnections[key] = distance;
            } else {
                cityConnections[key] = min(cityConnections[key], distance);
            }
        }
    }

    for (const auto &conn: cityConnections) {
        cityGraph.addEdge(conn.first.first, conn.first.second, conn.second, true);
    }

    cout << "Вершин (городов): " << cityGraph.getVertexCount() << endl;
    cout << "Ребер (маршрутов): " << cityGraph.getEdgeCount() << endl;

    // Алгоритм Дейкстры
    if (cityGraph.getVertexCount() >= 2) {
        auto vertices = cityGraph.getVertices();
        string startCity = vertices[0];
        string endCity = vertices.size() > 1 ? vertices[1] : vertices[0];

        for (const auto &v: vertices) {
            if (v.find("New_York") != string::npos) {
                startCity = v;
                break;
            }
        }

        for (const auto &v: vertices) {
            if (v != startCity &&
                (v.find("Los_Angeles") != string::npos ||
                 v.find("Chicago") != string::npos ||
                 v.find("Miami") != string::npos)) {
                endCity = v;
                break;
            }
        }

        cout << "\n=== АЛГОРИТМ ДЕЙКСТРЫ ===" << endl;
        cout << "Маршрут: " << startCity << " -> " << endCity << endl;

        auto dijkstra_start = steady_clock::now();
        auto result = cityGraph.dijkstra(startCity, endCity);
        auto dijkstra_end = steady_clock::now();
        double dijkstra_time = duration_cast<milliseconds>(dijkstra_end - dijkstra_start).count() / 1000.0;

        if (result.found) {
            cout << "Путь найден" << endl;
            cout << "Расстояние: " << fixed << setprecision(1) << result.distance << " миль" << endl;
            cout << "Городов в пути: " << result.path.size() << endl;
            cout << "Время выполнения: " << fixed << setprecision(3) << dijkstra_time << " сек" << endl;
        } else {
            cout << "Путь не найден" << endl;
        }
    }

    auto program_end = steady_clock::now();
    auto total_duration = duration_cast<milliseconds>(program_end - program_start);

    cout << "\n=== ИТОГО ===" << endl;
    cout << "Общее время выполнения: " << fixed << setprecision(2)
            << total_duration.count() / 1000.0 << " сек" << endl;

    return 0;
}
