#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_map>
#include <iostream>

class LZSSCompressor {
private:
    static const int WINDOW_SIZE = 4 * 1024;       // 4KB окно (12 бит)
    static const int MAX_MATCH_LENGTH = 18;        // 4 бита (3-18)
    static const int MIN_MATCH_LENGTH = 3;
    static const int HASH_SIZE = 3;

    struct Token {
        bool is_literal;
        uint16_t offset;
        uint8_t length;
        uint8_t literal;
    };

    uint32_t calculateHash(const std::vector<uint8_t>& data, size_t pos) {
        if (pos + HASH_SIZE > data.size()) return 0;
        return (data[pos] << 16) | (data[pos + 1] << 8) | data[pos + 2];
    }

    std::pair<uint16_t, uint8_t> findBestMatch(
        const std::vector<uint8_t>& data,
        size_t current_pos,
        std::unordered_map<uint32_t, std::vector<size_t>>& hash_table) {

        uint16_t best_offset = 0;
        uint8_t best_length = 0;

        if (current_pos + HASH_SIZE > data.size()) {
            return {0, 0};
        }

        uint32_t hash = calculateHash(data, current_pos);

        size_t window_start = (current_pos > WINDOW_SIZE) ?
                              current_pos - WINDOW_SIZE : 0;

        auto it = hash_table.find(hash);
        if (it != hash_table.end()) {
            for (size_t i : it->second) {
                if (i < window_start || i >= current_pos) continue;

                size_t length = 0;

                while (length < MAX_MATCH_LENGTH &&
                       current_pos + length < data.size() &&
                       data[i + length] == data[current_pos + length]) {
                    length++;
                }

                if (length >= MIN_MATCH_LENGTH && length > best_length) {
                    best_length = static_cast<uint8_t>(length);
                    best_offset = static_cast<uint16_t>(current_pos - i);

                    if (best_length >= MAX_MATCH_LENGTH) {
                        break;
                    }
                }
            }
        }

        return {best_offset, best_length};
    }

public:
    std::vector<uint8_t> compress(const std::vector<uint8_t>& input) {
        std::vector<Token> tokens;
        std::unordered_map<uint32_t, std::vector<size_t>> hash_table;

        size_t pos = 0;
        while (pos < input.size()) {
            auto [offset, length] = findBestMatch(input, pos, hash_table);

            if (pos + HASH_SIZE <= input.size()) {
                uint32_t hash = calculateHash(input, pos);
                hash_table[hash].push_back(pos);

                if (hash_table[hash].size() > 100) {
                    hash_table[hash].erase(hash_table[hash].begin());
                }
            }

            Token token = {};
            if (length >= MIN_MATCH_LENGTH && offset > 0) {
                token.is_literal = false;
                token.offset = offset;
                token.length = length;
                tokens.push_back(token);

                for (size_t i = 1; i < length && pos + i + HASH_SIZE <= input.size(); ++i) {
                    uint32_t h = calculateHash(input, pos + i);
                    hash_table[h].push_back(pos + i);
                    if (hash_table[h].size() > 100) {
                        hash_table[h].erase(hash_table[h].begin());
                    }
                }

                pos += length;
            } else {
                if (length >= MIN_MATCH_LENGTH && offset == 0) {
                    // DEBUG: обнаружен недопустимый токен
                    std::cerr << "WARNING: Skipping invalid match at pos " << pos
                              << " (offset=0, length=" << (int)length << ")" << std::endl;
                }
                token.is_literal = true;
                token.literal = input[pos];
                tokens.push_back(token);
                pos++;
            }
        }

        return encodeTokens(tokens);
    }

    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed) {
        std::vector<Token> tokens = decodeTokens(compressed);
        std::vector<uint8_t> result;

        for (const auto& token : tokens) {
            if (token.is_literal) {
                result.push_back(token.literal);
            } else {
                size_t copy_pos = result.size() - token.offset;
                for (uint8_t i = 0; i < token.length; ++i) {
                    result.push_back(result[copy_pos + i]);
                }
            }
        }

        return result;
    }

    double getCompressionRatio(size_t original_size, size_t compressed_size) const {
        return 100.0 * (1.0 - (double)compressed_size / original_size);
    }

private:
    std::vector<uint8_t> encodeTokens(const std::vector<Token>& tokens) {
        std::vector<uint8_t> result;

        // Сохраняем количество токенов (4 байта, big-endian)
        uint32_t token_count = static_cast<uint32_t>(tokens.size());
        result.push_back((token_count >> 24) & 0xFF);
        result.push_back((token_count >> 16) & 0xFF);
        result.push_back((token_count >> 8) & 0xFF);
        result.push_back(token_count & 0xFF);

        size_t i = 0;
        while (i < tokens.size()) {
            size_t flags_pos = result.size();
            result.push_back(0);

            uint8_t flags = 0;

            for (int bit = 0; bit < 8 && i < tokens.size(); ++bit, ++i) {
                const Token& token = tokens[i];

                if (token.is_literal) {
                    result.push_back(token.literal);
                } else {
                    flags |= (1 << bit);

                    uint8_t len_encoded = token.length - MIN_MATCH_LENGTH;
                    if (len_encoded > 15) len_encoded = 15;

                    // offset должен помещаться в 12 бит
                    uint16_t offset_clamped = token.offset & 0x0FFF;
                    uint16_t encoded = (offset_clamped << 4) | len_encoded;
                    result.push_back(static_cast<uint8_t>(encoded >> 8));
                    result.push_back(static_cast<uint8_t>(encoded & 0xFF));
                }
            }

            result[flags_pos] = flags;
        }

        return result;
    }

    std::vector<Token> decodeTokens(const std::vector<uint8_t>& data) {
        std::vector<Token> tokens;

        if (data.size() < 4) return tokens;

        // Читаем количество токенов (4 байта, big-endian)
        uint32_t token_count = (static_cast<uint32_t>(data[0]) << 24) |
                               (static_cast<uint32_t>(data[1]) << 16) |
                               (static_cast<uint32_t>(data[2]) << 8) |
                               static_cast<uint32_t>(data[3]);

        size_t pos = 4;
        uint32_t tokens_read = 0;

        while (pos < data.size() && tokens_read < token_count) {
            uint8_t flags = data[pos++];

            for (int bit = 0; bit < 8 && tokens_read < token_count; ++bit) {
                Token token = {};

                if (flags & (1 << bit)) {
                    // Ссылка: нужно 2 байта
                    if (pos + 2 > data.size()) return tokens;

                    uint16_t encoded = (static_cast<uint16_t>(data[pos]) << 8) | data[pos + 1];
                    pos += 2;

                    token.is_literal = false;
                    token.offset = encoded >> 4;
                    token.length = (encoded & 0x0F) + MIN_MATCH_LENGTH;
                } else {
                    // Литерал: нужен 1 байт
                    if (pos >= data.size()) return tokens;

                    token.is_literal = true;
                    token.literal = data[pos++];
                }

                tokens.push_back(token);
                tokens_read++;
            }
        }

        return tokens;
    }
};

void compressFileStreaming(const std::string& input_file, const std::string& output_file);
void decompressFileStreaming(const std::string& input_file, const std::string& output_file);

#endif
