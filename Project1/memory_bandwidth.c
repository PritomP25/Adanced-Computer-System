#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MEMORY_SIZE 64 * 1024 * 1024  // 64MB of memory for testing

// Function to measure bandwidth
double measure_bandwidth(int *array, size_t size, size_t granularity, double read_ratio, int iterations) {
    struct timespec start, end;
    volatile int temp = 0;  // Volatile to avoid compiler optimization
    int reads = (int)(iterations * read_ratio);
    int writes = iterations - reads;
    
    // Get start time
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (size_t i = 0; i < iterations; i++) {
        if (i < reads) {
            for (size_t j = 0; j < size; j += granularity) {
                temp = array[j];  // Read memory
            }
        } else {
            for (size_t j = 0; j < size; j += granularity) {
                array[j] = j;  // Write memory
            }
        }
    }
    
    // Get end time
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calculate total time in seconds
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    
    // Calculate bandwidth in bytes per second
    double bytes_accessed = iterations * (size / granularity) * granularity;
    return bytes_accessed / time_taken;
}

int main() {
    // Allocate memory for the test
    int *memory_array = (int *)malloc(MEMORY_SIZE * sizeof(int));
    if (memory_array == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Data access granularities
    size_t granularities[] = {64, 256, 1024};
    
    // Read/Write ratios: 100% read, 100% write, 70% read/30% write, 50% read/50% write
    double ratios[] = {1.0, 0.0, 0.7, 0.5};
    const char *ratio_labels[] = {"100% Read", "100% Write", "70% Read / 30% Write", "50% Read / 50% Write"};
    
    // Number of iterations for each test
    int iterations = 1000;
    
    // Loop through different granularities and ratios
    for (size_t g = 0; g < 3; g++) {
        printf("\nData Granularity: %zu bytes\n", granularities[g]);
        for (size_t r = 0; r < 4; r++) {
            double bandwidth = measure_bandwidth(memory_array, MEMORY_SIZE, granularities[g], ratios[r], iterations);
            printf("%s Bandwidth: %.2f MB/s\n", ratio_labels[r], bandwidth / (1024 * 1024));
        }
    }
    
    // Free memory
    free(memory_array);
    
    return 0;
}
