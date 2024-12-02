#include <opencv2/opencv.hpp>
#include <vector>
#include <chrono>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <immintrin.h>  // For SIMD intrinsics (e.g., AVX)

// SIMD-based downscaling function (optimizing resizing)
void compressFrame_SIMD(cv::Mat &frame, int method_enable) {
    // Check if frame is empty
    if (frame.empty()) {
        std::cerr << "Error: Frame is empty, skipping compression.\n";
        return;  // Skip this frame
    }

    // Compression scale (reduce resolution by this factor)
    int compression_scale = 2;

    // Store original frame dimensions
    cv::Size original_size = frame.size();

    // Downsample the frame using SIMD (custom implementation)
    cv::Mat compressed_frame;

    // Check if resizing is possible (ensure valid dimensions)
    if (frame.cols > 0 && frame.rows > 0) {
        cv::resize(frame, compressed_frame, cv::Size(frame.cols / compression_scale, frame.rows / compression_scale));
    } else {
        std::cerr << "Error: Invalid frame dimensions.\n";
        return;  // Skip invalid frame
    }

    // Use SIMD to process each pixel (example with AVX for 32-bit float pixel data)
    if (method_enable == 2) {
        #pragma omp parallel for
        for (int y = 0; y < compressed_frame.rows; y++) {
            for (int x = 0; x < compressed_frame.cols; x++) {
                cv::Vec3b &pixel = compressed_frame.at<cv::Vec3b>(y, x);
                __m256i pixel_simd = _mm256_set_epi8(
                    pixel[0], pixel[1], pixel[2], 0, 0, 0, 0, 0,
                    pixel[0], pixel[1], pixel[2], 0, 0, 0, 0, 0,
                    pixel[0], pixel[1], pixel[2], 0, 0, 0, 0, 0,
                    pixel[0], pixel[1], pixel[2], 0, 0, 0, 0, 0
                );
                // Perform SIMD operation (e.g., apply filter, adjust pixel values)
            }
        }
    }

    // Ensure to write the processed frame if it's valid
    if (!compressed_frame.empty()) {
        frame = compressed_frame;
    }
}



// Shared variables for the producer-consumer model
std::queue<cv::Mat> frame_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;
std::atomic<bool> done_reading(false); // Indicates when the producer has finished


// Spatial compression function
void compressFrame(cv::Mat &frame, int method_enable) {
    // Compression scale (reduce resolution by this factor)
    int compression_scale = 2;

    // Store original frame dimensions
    cv::Size original_size = frame.size();

    // Downsample the frame
    cv::Mat compressed_frame;
    cv::resize(frame, compressed_frame, cv::Size(frame.cols / compression_scale, frame.rows / compression_scale));

    // For demonstration purposes, upsample back to the original size
    cv::resize(compressed_frame, frame, original_size);
}


// Producer function: Reads frames and adds them to the queue
void producer(cv::VideoCapture &video) {
    cv::Mat frame;
    while (video.read(frame)) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            frame_queue.push(frame.clone()); // Add a copy to the queue
        }
        queue_cv.notify_one(); // Notify one worker thread
    }
    done_reading = true;
    queue_cv.notify_all(); // Notify all workers that reading is complete
}

// Worker function: Processes frames from the queue
void worker(int thread_id, int method_enable, cv::VideoWriter &output) {
    while (true) {
        cv::Mat frame;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [] { return !frame_queue.empty() || done_reading; });
            if (frame_queue.empty() && done_reading) break; // Exit if all frames are processed
            frame = frame_queue.front();
            frame_queue.pop();
        }
        compressFrame(frame, method_enable); // Process frame
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            output.write(frame); // Write processed frame
        }
    }
}

// Main multi-threading function
void processVideoMultiThreaded(cv::VideoCapture &video, cv::VideoWriter &output, int num_threads) {
    // Launch producer thread
    std::thread producer_thread(producer, std::ref(video));

    // Launch worker threads
    std::vector<std::thread> worker_threads;
    for (int i = 0; i < num_threads; ++i) {
        worker_threads.emplace_back(worker, i, 1, std::ref(output));
    }

    // Wait for threads to complete
    producer_thread.join();
    for (auto &t : worker_threads) {
        t.join();
    }
}



int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: ./proj5.out <filename>.mp4 <num_threads> <method_enable>\n";
        return -1;
    }

    std::string filename = argv[1];
    int num_threads = std::stoi(argv[2]);
    int method_enable = std::stoi(argv[3]);

    cv::VideoCapture video(filename);
    if (!video.isOpened()) {
        std::cerr << "Error: Cannot open video file " << filename << "\n";
        return -1;
    }

    int frame_width = video.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = video.get(cv::CAP_PROP_FRAME_HEIGHT);
    int fps = video.get(cv::CAP_PROP_FPS);

    cv::VideoWriter output("compressed_" + filename,
                       cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                       fps, cv::Size(frame_width, frame_height));

    if (!output.isOpened()) {
        std::cerr << "Error: Cannot create output file\n";
        return -1;
    }

    cv::Mat frame;
    auto start = std::chrono::high_resolution_clock::now();

    if (method_enable == 1) {
        processVideoMultiThreaded(video, output, num_threads);
    } else if (method_enable == 2) { 
        while (video.read(frame)) {
        if (frame.empty()) {
            std::cerr << "Warning: Empty frame detected, using fallback.\n";
            // Fill the frame with a black frame as fallback
            frame = cv::Mat::zeros(frame.size(), frame.type());
        }

        compressFrame_SIMD(frame, method_enable);  // Process frame
        output.write(frame);  // Write the processed frame to output
    }
    } else {
        while (video.read(frame)) {
            compressFrame(frame, method_enable);
            output.write(frame);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Processing time: " << elapsed.count() << " seconds\n";

    video.release();
    output.release();

    return 0;
}
