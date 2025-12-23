#ifndef READING_BY_INSTANCES_H
#define READING_BY_INSTANCES_H

#include <string>
#include <vector>
#include <unordered_set>
#include "flight.h"

// Вспомогательные функции парсинга
std::string parse_line(const std::string& line);
std::vector<std::string> sep_line(const std::string& line);

// Функции чтения, возвращающие данные
// max_lines = 0 означает загрузить весь файл
std::unordered_set<flight> read_flights_by_instances(const std::string& filename, bool show_progress = false, size_t max_lines = 0);
std::unordered_set<flight> read_flights_by_strings(const std::string& filename, bool show_progress = false, size_t max_lines = 0);
std::unordered_set<flight> read_flights_by_library(const std::string& filename, bool show_progress = false, size_t max_lines = 0);

// Функция для получения размера файла
size_t get_file_size(const std::string& filename);

// Функция сохранения
void save_unique_keys_to_csv(const std::unordered_set<flight>& flights, const std::string& filename);

#endif // READING_BY_INSTANCES_H
