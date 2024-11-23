#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono> // For timing
#include <immintrin.h> // For SIMD

// Global dictionary for reverse lookup
std::unordered_map<int, std::string> reverse_dictionary;

// Load encoded file and dictionary into memory
bool load_encoded_file(const std::string &encoded_file, std::unordered_map<int, std::string> &reverse_dict, std::vector<int> &encoded_data) {
    std::ifstream infile(encoded_file);
    if (!infile.is_open()) {
        std::cerr << "Error opening encoded file!" << std::endl;
        return false;
    }

    std::string line;
    bool is_dict_section = true;

    while (std::getline(infile, line)) {
        if (line == "Encoded Data:") {
            is_dict_section = false;
            continue;
        }

        if (is_dict_section) {
            size_t comma_pos = line.find(',');
            if (comma_pos != std::string::npos) {
                std::string key = line.substr(0, comma_pos);
                int value = std::stoi(line.substr(comma_pos + 1));
                reverse_dict[value] = key;
            }
        } else {
            encoded_data.push_back(std::stoi(line));
        }
    }

    infile.close();
    return true;
}

// Exact match query
void exact_match_query(const std::vector<int> &encoded_data, const std::unordered_map<int, std::string> &reverse_dict, const std::string &query) {
    auto it = std::find_if(reverse_dict.begin(), reverse_dict.end(), [&](const auto &entry) {
        return entry.second == query;
    });

    if (it == reverse_dict.end()) {
        std::cout << "Item not found." << std::endl;
        return;
    }

    int query_id = it->first;
    std::cout << "Indices of '" << query << "': ";
    for (size_t i = 0; i < encoded_data.size(); ++i) {
        if (encoded_data[i] == query_id) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
}

// Prefix match query
void prefix_match_query(const std::vector<int> &encoded_data, const std::unordered_map<int, std::string> &reverse_dict, const std::string &prefix) {
    std::unordered_map<int, std::vector<size_t>> matching_entries;

    for (size_t i = 0; i < encoded_data.size(); ++i) {
        int encoded_value = encoded_data[i];

        auto it = reverse_dict.find(encoded_value);
        if (it == reverse_dict.end()) {
            continue; // Skip if the key doesn't exist
        }

        const std::string &entry = it->second;
        if (entry.substr(0, prefix.size()) == prefix) {
            matching_entries[encoded_value].push_back(i);
        }
    }

    if (matching_entries.empty()) {
        std::cout << "No entries found with prefix '" << prefix << "'." << std::endl;
        return;
    }

    for (const auto &entry : matching_entries) {
        const std::string &value = reverse_dict.at(entry.first);
        std::cout << "Prefix Match: '" << value << "' at indices: ";
        for (size_t idx : entry.second) {
            std::cout << idx << " ";
        }
        std::cout << std::endl;
    }
}

// Vanilla exact match query
void vanilla_exact_match(const std::vector<int> &encoded_data, const std::unordered_map<int, std::string> &reverse_dict, const std::string &query) {
    exact_match_query(encoded_data, reverse_dict, query);
}

// Vanilla prefix match query
void vanilla_prefix_match(const std::vector<int> &encoded_data, const std::unordered_map<int, std::string> &reverse_dict, const std::string &prefix) {
    prefix_match_query(encoded_data, reverse_dict, prefix);
}

// SIMD-accelerated exact match query
void simd_exact_match(const std::vector<int> &encoded_data, const std::unordered_map<int, std::string> &reverse_dict, const std::string &query) {
    auto it = std::find_if(reverse_dict.begin(), reverse_dict.end(), [&](const auto &entry) {
        return entry.second == query;
    });

    if (it == reverse_dict.end()) {
        std::cout << "Item not found in SIMD exact match." << std::endl;
        return;
    }

    int query_id = it->first;
    size_t data_size = encoded_data.size();
    size_t simd_size = 8; // SIMD processes 8 integers at a time

    std::cout << "Indices of '" << query << "' using SIMD exact match: ";
    __m256i simd_query_id = _mm256_set1_epi32(query_id); // Broadcast query_id to all lanes
    for (size_t i = 0; i < data_size; i += simd_size) {
        __m256i simd_data = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(&encoded_data[i])); // Load 8 integers
        __m256i cmp_result = _mm256_cmpeq_epi32(simd_data, simd_query_id); // Compare 8 integers
        int mask = _mm256_movemask_ps(_mm256_castsi256_ps(cmp_result)); // Get comparison result as a bitmask

        for (int bit = 0; bit < simd_size; ++bit) {
            if (mask & (1 << bit)) {
                std::cout << (i + bit) << " ";
            }
        }
    }
    std::cout << std::endl;
}

// SIMD-accelerated prefix match query
void simd_prefix_match(const std::vector<int> &encoded_data, const std::unordered_map<int, std::string> &reverse_dict, const std::string &prefix) {
    size_t data_size = encoded_data.size();
    size_t simd_size = 8; // SIMD processes 8 integers at a time

    std::cout << "Performing SIMD prefix match..." << std::endl;
    for (size_t i = 0; i < data_size; i += simd_size) {
        __m256i simd_data = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(&encoded_data[i])); // Load 8 integers
        int simd_array[8];
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(simd_array), simd_data); // Store back to an array for sequential lookup

        for (int j = 0; j < 8 && (i + j) < data_size; ++j) {
            int encoded_value = simd_array[j];
            auto it = reverse_dict.find(encoded_value);
            if (it != reverse_dict.end()) {
                const std::string &entry = it->second;
                if (entry.substr(0, prefix.size()) == prefix) {
                    std::cout << "Prefix Match: '" << entry << "' at index: " << (i + j) << std::endl;
                }
            }
        }
    }
}

// Query function for all modes
void query_encoded_file(const std::string &encoded_file, int query_mode, const std::string &query_value) {
    std::unordered_map<int, std::string> reverse_dict;
    std::vector<int> encoded_data;

    if (!load_encoded_file(encoded_file, reverse_dict, encoded_data)) {
        return;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    switch (query_mode) {
        case 1:
            exact_match_query(encoded_data, reverse_dict, query_value);
            break;
        case 2:
            prefix_match_query(encoded_data, reverse_dict, query_value);
            break;
        case 3:
            vanilla_exact_match(encoded_data, reverse_dict, query_value);
            break;
        case 4:
            vanilla_prefix_match(encoded_data, reverse_dict, query_value);
            break;
        case 5:
            simd_exact_match(encoded_data, reverse_dict, query_value);
            break;
        case 6:
            simd_prefix_match(encoded_data, reverse_dict, query_value);
            break;
        default:
            std::cerr << "Invalid query mode!" << std::endl;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> query_duration = end_time - start_time;
    std::cout << "Query Time: " << query_duration.count() << " seconds\n";
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " query <encoded_file> <query_mode> <query_value>" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    if (command == "query") {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " query <encoded_file> <query_mode> <query_value>" << std::endl;
            return 1;
        }

        std::string encoded_file = argv[2];
        int query_mode = std::stoi(argv[3]);
        std::string query_value = argv[4];

        query_encoded_file(encoded_file, query_mode, query_value);
    } else {
        std::cerr << "Invalid command! Supported commands: query." << std::endl;
    }

    return 0;
}
