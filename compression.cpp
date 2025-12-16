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
    
    while (in.read(reinterpret_cast<char*>(block.data()), BLOCK_SIZE) || 
           in.gcount() > 0) {
        size_t bytes_read = in.gcount();
        block.resize(bytes_read);
        
        // Сжимаем блок
        auto compressed = compressor.compress(block);
        
        // Сохраняем размер блока и сами данные
        uint32_t compressed_size = compressed.size();
        out.write(reinterpret_cast<const char*>(&compressed_size), 4);
        out.write(reinterpret_cast<const char*>(compressed.data()), 
                 compressed.size());
        
        total_original += bytes_read;
        total_compressed += compressed.size() + 4;
        
        std::cout << "Обработано: " << (total_original / 1024 / 1024) 
                  << "MB" << std::endl;
        
        block.resize(BLOCK_SIZE);
    }
    
    std::cout << std::endl << "=== Итоги ===" << std::endl;
    std::cout << "Original size: " << (total_original / 1024 / 1024) 
              << "MB" << std::endl;
    std::cout << "Compressed size: " << (total_compressed / 1024 / 1024) 
              << "MB" << std::endl;
    std::cout << "Compression: " 
              << 100.0 * (1.0 - (double)total_compressed / total_original) 
              << "%" << std::endl;
}

int main() {
    compressFileStreaming("flights.csv", "flights.csv.lzss");
    return 0;
}