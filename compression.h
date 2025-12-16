#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

class LZSSCompressor {
private:
    static const int WINDOW_SIZE = 32 * 1024;      // 32KB окно
    static const int MAX_MATCH_LENGTH = 258;   // макс длина совпадения
    static const int MIN_MATCH_LENGTH = 3;     // мин длина для кодирования
    
    struct Token {
        bool is_literal;           // true = литерал, false = ссылка
        uint16_t offset;           // расстояние до совпадения
        uint8_t length;            // длина совпадения
        uint8_t literal;           // символ (если литерал)
    };
    
    // Поиск лучшего совпадения в окне
    std::pair<uint16_t, uint8_t> findBestMatch(
        const std::vector<uint8_t>& data,
        size_t current_pos) {
        
        uint16_t best_offset = 0;
        uint8_t best_length = 0;
        
        // Граница окна
        size_t window_start = (current_pos > WINDOW_SIZE) ? 
                              current_pos - WINDOW_SIZE : 0;
        
        // Ищем совпадения в окне
        for (size_t i = window_start; i < current_pos; ++i) {
            uint8_t length = 0;
            
            while (length < MAX_MATCH_LENGTH && 
                   current_pos + length < data.size() && 
                   data[i + length] == data[current_pos + length]) {
                length++;
            }
            
            // Обновляем лучшее совпадение
            if (length >= MIN_MATCH_LENGTH && length > best_length) {
                best_length = length;
                best_offset = current_pos - i;
            }
        }
        
        return {best_offset, best_length};
    }
    
public:
    // Сжатие потока
    std::vector<uint8_t> compress(const std::vector<uint8_t>& input) {
        std::vector<Token> tokens;
        std::vector<uint8_t> result;
        
        size_t pos = 0;
        while (pos < input.size()) {
            auto [offset, length] = findBestMatch(input, pos);
            
            Token token;
            if (length >= MIN_MATCH_LENGTH) {
                // Кодируем ссылку на совпадение
                token.is_literal = false;
                token.offset = offset;
                token.length = length;
                tokens.push_back(token);
                pos += length;
            } else {
                // Кодируем литерал
                token.is_literal = true;
                token.literal = input[pos];
                tokens.push_back(token);
                pos++;
            }
        }
        
        // Кодируем токены в бинарный формат
        return encodeTokens(tokens);
    }
    
    // Распаковка потока
    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed) {
        std::vector<Token> tokens = decodeTokens(compressed);
        std::vector<uint8_t> result;
        
        for (const auto& token : tokens) {
            if (token.is_literal) {
                result.push_back(token.literal);
            } else {
                // Копируем из истории
                size_t copy_pos = result.size() - token.offset;
                for (uint8_t i = 0; i < token.length; ++i) {
                    result.push_back(result[copy_pos + i]);
                }
            }
        }
        
        return result;
    }
    
    double getCompressionRatio(size_t original_size, 
                               size_t compressed_size) const {
        return 100.0 * (1.0 - (double)compressed_size / original_size);
    }
    
private:
    // Кодирование токенов в бинарный формат
    std::vector<uint8_t> encodeTokens(const std::vector<Token>& tokens) {
        std::vector<uint8_t> result;
        
        // Сохраняем количество токенов
        uint32_t count = tokens.size();
        result.resize(4);
        memcpy(result.data(), &count, 4);
        
        // Кодируем каждый токен (4 байта)
        for (const auto& token : tokens) {
            uint32_t encoded = 0;
            
            if (token.is_literal) {
                // Бит 31 = 0 (литерал)
                // Биты 0-7 = символ
                encoded = token.literal;
            } else {
                // Бит 31 = 1 (ссылка)
                // Биты 15-30 = offset
                // Биты 0-7 = length
                encoded = (1u << 31) | (token.offset << 8) | token.length;
            }
            
            result.resize(result.size() + 4);
            memcpy(&result[result.size()-4], &encoded, 4);
        }
        
        return result;
    }
    
    std::vector<Token> decodeTokens(const std::vector<uint8_t>& data) {
        std::vector<Token> tokens;
        
        if (data.size() < 4) return tokens;
        
        uint32_t count;
        memcpy(&count, data.data(), 4);
        
        for (uint32_t i = 0; i < count && (4 + i*4 + 4) <= data.size(); ++i) {
            uint32_t encoded;
            memcpy(&encoded, &data[4 + i*4], 4);
            
            Token token;
            if ((encoded >> 31) == 0) {
                // Литерал
                token.is_literal = true;
                token.literal = encoded & 0xFF;
            } else {
                // Ссылка
                token.is_literal = false;
                token.offset = (encoded >> 8) & 0xFFFF;
                token.length = encoded & 0xFF;
            }
            tokens.push_back(token);
        }
        
        return tokens;
    }
};

void compressFileStreaming(const std::string& input_file, const std::string& output_file);


#endif