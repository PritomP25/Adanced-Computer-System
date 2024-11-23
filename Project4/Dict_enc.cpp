#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <chrono>
#include <condition_variable>

// Global dictionary and encoded data
std::unordered_map<std::string, int> dictionary;
std::mutex dict_mutex;
std::atomic<int> current_id{0};

// Thread-safe queue for dynamic chunking
std::queue<std::vector<std::string>> task_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;
bool producer_done = false;

// Thread-local encoded data
std::vector<std::vector<int>> thread_local_encoded_data;

// Thread-safe function to encode a line and update dictionary
void encode_line(const std::string &line, std::unordered_map<std::string, int> &local_dict, std::vector<int> &local_encoded_data) {
    int id;
    {
        auto it = local_dict.find(line);
        if (it == local_dict.end()) {
            id = current_id.fetch_add(1, std::memory_order_relaxed);
            local_dict[line] = id;
        } else {
            id = it->second;
        }
    }
    local_encoded_data.push_back(id);
}

// Function to process a file chunk (dictionary mode)
void process_chunk_dict(const std::vector<std::string> &chunk, std::unordered_map<std::string, int> &local_dict, std::vector<int> &local_encoded_data) {
    for (const auto &line : chunk) {
        encode_line(line, local_dict, local_encoded_data);
    }
}

// Function to process a file chunk (vanilla mode)
void process_chunk_vanilla(const std::vector<std::string> &chunk, std::ofstream &outfile) {
    for (const auto &line : chunk) {
        outfile << line << "\n";
    }
}

// Worker thread function for dictionary mode
void worker_thread_dict(int thread_id) {
    std::unordered_map<std::string, int> local_dict; // Thread-local dictionary
    std::vector<int> local_encoded_data;            // Thread-local encoded data

    while (true) {
        std::vector<std::string> chunk;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [] { return !task_queue.empty() || producer_done; });

            if (task_queue.empty()) break;
            chunk = std::move(task_queue.front());
            task_queue.pop();
        }
        process_chunk_dict(chunk, local_dict, local_encoded_data);
    }

    // Merge thread-local dictionary with global dictionary
    {
        std::lock_guard<std::mutex> lock(dict_mutex);
        for (const auto &entry : local_dict) {
            if (dictionary.find(entry.first) == dictionary.end()) {
                dictionary[entry.first] = entry.second;
            }
        }
    }

    // Store thread-local encoded data in global structure
    {
        std::lock_guard<std::mutex> lock(dict_mutex);
        thread_local_encoded_data[thread_id] = std::move(local_encoded_data);
    }
}

// Main function to process the file in chunks
void process_file_in_chunks(const std::string &input_file, const std::string &output_file, int num_threads, const std::string &mode) {
    std::ifstream infile(input_file);
    if (!infile.is_open()) {
        std::cerr << "Error opening input file!" << std::endl;
        return;
    }

    std::ofstream outfile(output_file);
    if (!outfile.is_open()) {
        std::cerr << "Error opening output file!" << std::endl;
        return;
    }

    if (mode == "dict") {
        thread_local_encoded_data.resize(num_threads);

        // Start timer
        auto start_time = std::chrono::high_resolution_clock::now();

        // Producer: Read input file and create chunks
        std::thread producer_thread([&]() {
            std::string line;
            std::vector<std::string> chunk;
            size_t chunk_size = 10000; // Adjust chunk size for performance

            while (std::getline(infile, line)) {
                chunk.push_back(line);
                if (chunk.size() >= chunk_size) {
                    {
                        std::lock_guard<std::mutex> lock(queue_mutex);
                        task_queue.push(std::move(chunk));
                    }
                    queue_cv.notify_one();
                    chunk.clear();
                }
            }

            if (!chunk.empty()) {
                {
                    std::lock_guard<std::mutex> lock(queue_mutex);
                    task_queue.push(std::move(chunk));
                }
                queue_cv.notify_one();
            }

            producer_done = true;
            queue_cv.notify_all();
        });

        // Consumer threads
        std::vector<std::thread> worker_threads;
        for (int i = 0; i < num_threads; ++i) {
            worker_threads.emplace_back(worker_thread_dict, i);
        }

        producer_thread.join();
        for (auto &thread : worker_threads) {
            thread.join();
        }

        // Stop timer
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> encoding_duration = end_time - start_time;
        std::cout << "Encoding Time (Dictionary Mode): " << encoding_duration.count() << " seconds\n";

        // Write dictionary and encoded data
        outfile << "Dictionary:\n";
        for (const auto &entry : dictionary) {
            outfile << entry.first << "," << entry.second << "\n";
        }

        outfile << "Encoded Data:\n";
        for (const auto &local_data : thread_local_encoded_data) {
            for (const auto &id : local_data) {
                outfile << id << "\n";
            }
        }
    } else if (mode == "vanilla") {
        // Vanilla mode: process and write each line as is
        auto start_time = std::chrono::high_resolution_clock::now();

        std::string line;
        while (std::getline(infile, line)) {
            outfile << line << "\n";
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> scan_duration = end_time - start_time;
        std::cout << "Processing Time (Vanilla Mode): " << scan_duration.count() << " seconds\n";
    } else {
        std::cerr << "Invalid mode! Use 'dict' or 'vanilla'." << std::endl;
        return;
    }

    infile.close();
    outfile.close();
}

int main(int argc, char **argv) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <num_threads> <dict|vanilla>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    int num_threads = std::stoi(argv[3]);
    std::string mode = argv[4];

    process_file_in_chunks(input_file, output_file, num_threads, mode);

    return 0;
}
