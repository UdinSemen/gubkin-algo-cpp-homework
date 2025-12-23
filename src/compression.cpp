#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>

#include "compression.h"

void compressFileStreaming(const std::string& input_file,
                          const std::string& output_file) {
    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary);

    const size_t BLOCK_SIZE = 1024 * 1024; // 1MB блоки
    LZSSCompressor compressor;

    std::vector<uint8_t> block(BLOCK_SIZE);
    uint64_t total_original = 0;
    uint64_t total_compressed = 0;

    int block_num = 0;
    while (in.read(reinterpret_cast<char*>(block.data()), BLOCK_SIZE) ||
           in.gcount() > 0) {
        size_t bytes_read = in.gcount();
        block.resize(bytes_read);

        // Сжимаем блок
        auto compressed = compressor.compress(block);

        // ВАЖНО: Сохраняем ОБА размера - оригинального и сжатого блока
        uint32_t original_size = static_cast<uint32_t>(bytes_read);
        uint32_t compressed_size = static_cast<uint32_t>(compressed.size());

        out.write(reinterpret_cast<const char*>(&original_size), 4);
        out.write(reinterpret_cast<const char*>(&compressed_size), 4);
        out.write(reinterpret_cast<const char*>(compressed.data()),
                 compressed.size());

        total_original += bytes_read;
        total_compressed += compressed.size() + 8; // +8 для двух uint32_t
        block_num++;

        if (block_num % 50 == 0 || bytes_read < BLOCK_SIZE) {
            std::cout << "  Блок " << block_num << ": " << bytes_read << " -> "
                     << compressed.size() << " байт | Всего: "
                     << (total_original / 1024 / 1024) << " MB" << std::endl;
        }

        block.resize(BLOCK_SIZE);
    }

    std::cout << "\n  Всего блоков обработано: " << block_num << std::endl;

    std::cout << std::endl << "=== Итоги ===" << std::endl;
    std::cout << "Original size: " << (total_original / 1024 / 1024)
              << "MB" << std::endl;
    std::cout << "Compressed size: " << (total_compressed / 1024 / 1024)
              << "MB" << std::endl;
    std::cout << "Compression: "
              << 100.0 * (1.0 - (double)total_compressed / total_original)
              << "%" << std::endl;
}

void decompressFileStreaming(const std::string& input_file,
                             const std::string& output_file) {
    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary);

    if (!in.is_open()) {
        std::cerr << "Cannot open compressed file: " << input_file << std::endl;
        return;
    }

    if (!out.is_open()) {
        std::cerr << "Cannot create output file: " << output_file << std::endl;
        return;
    }

    LZSSCompressor compressor;
    uint64_t total_decompressed = 0;
    uint64_t total_compressed = 0;
    int block_num = 0;

    while (true) {
        // Читаем размер оригинального блока
        uint32_t original_size = 0;
        in.read(reinterpret_cast<char*>(&original_size), 4);

        if (in.gcount() != 4) {
            // Достигнут конец файла
            break;
        }

        // Читаем размер сжатого блока
        uint32_t compressed_size = 0;
        in.read(reinterpret_cast<char*>(&compressed_size), 4);

        if (in.gcount() != 4) {
            std::cerr << "Unexpected end of file while reading compressed size" << std::endl;
            break;
        }

        if (compressed_size == 0 || compressed_size > 10 * 1024 * 1024) {
            std::cerr << "Invalid compressed block size: " << compressed_size << std::endl;
            break;
        }

        if (original_size == 0 || original_size > 10 * 1024 * 1024) {
            std::cerr << "Invalid original block size: " << original_size << std::endl;
            break;
        }

        // Читаем сжатые данные
        std::vector<uint8_t> compressed_block(compressed_size);
        in.read(reinterpret_cast<char*>(compressed_block.data()), compressed_size);

        if (in.gcount() != static_cast<std::streamsize>(compressed_size)) {
            std::cerr << "Unexpected end of file while reading block data" << std::endl;
            break;
        }

        // Распаковываем блок
        auto decompressed = compressor.decompress(compressed_block);

        // Проверяем, что размер совпадает
        if (decompressed.size() != original_size) {
            std::cerr << "  ПРЕДУПРЕЖДЕНИЕ на блоке " << block_num + 1 << ":" << std::endl;
            std::cerr << "     Размер после распаковки: " << decompressed.size() << std::endl;
            std::cerr << "     Ожидаемый размер: " << original_size << std::endl;
            std::cerr << "     Размер сжатого блока: " << compressed_size << std::endl;

            // Это критическая ошибка - размеры должны совпадать
            if (decompressed.size() < original_size) {
                std::cerr << "  КРИТИЧЕСКАЯ ОШИБКА: Потеря данных при распаковке!" << std::endl;
            }
        }

        // Записываем распакованные данные (используем точный размер оригинала)
        size_t write_size = std::min(decompressed.size(), static_cast<size_t>(original_size));
        out.write(reinterpret_cast<const char*>(decompressed.data()), write_size);

        total_compressed += compressed_size + 8;
        total_decompressed += write_size;
        block_num++;

        if (block_num % 50 == 0) {
            std::cout << "  Распаковано " << block_num << " блоков | Размер: "
                     << (total_decompressed / 1024 / 1024) << " MB" << std::endl;
        }

        // Логируем последний блок
        if (original_size < 1024 * 1024) {
            std::cout << "  Последний блок " << block_num << ": "
                     << compressed_size << " -> " << write_size << " байт" << std::endl;
        }
    }

    in.close();
    out.close();

    std::cout << std::endl << "=== Итоги распаковки ===" << std::endl;
    std::cout << "Сжатый размер: " << (total_compressed / 1024 / 1024)
              << " MB" << std::endl;
    std::cout << "Распакованный размер: " << (total_decompressed / 1024 / 1024)
              << " MB" << std::endl;
    std::cout << "Обработано блоков: " << block_num << std::endl;
    std::cout << "Файл сохранен: " << output_file << std::endl;
}