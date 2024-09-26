#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CACHE_SIZE 64 * 1024      // 64KB, simulating cache-size
#define MEMORY_SIZE 64 * 1024 * 1024 // 64MB, simulating main memory-size

// Function to measure latency
double measure_latency(int *array, size_t size, int is_write) {
    struct timespec start, end;
    volatile int temp = 0;  // Volatile to prevent compiler optimization
    
    // Get start time
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (size_t i = 0; i < size; i += 64) {  // Access every 64 bytes (cache line)
        if (is_write) {
            array[i] = i;  // Write to memory
        } else {
            temp = array[i];  // Read from memory
        }
    }
    
    // Get end time
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate latency in nanoseconds
    double time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken += (end.tv_nsec - start.tv_nsec);
    
    return time_taken / (size / 64);  // Average latency per access
}

int main() {
    // Allocate arrays for cache and memory tests
    int *cache_array = (int *)malloc(CACHE_SIZE * sizeof(int));
    int *memory_array = (int *)malloc(MEMORY_SIZE * sizeof(int));
    
    if (cache_array == NULL || memory_array == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Measure read/write latency for cache-sized array
    printf("Cache Latency (Read): %.2f ns\n", measure_latency(cache_array, CACHE_SIZE, 0));
    printf("Cache Latency (Write): %.2f ns\n", measure_latency(cache_array, CACHE_SIZE, 1));
    
    // Measure read/write latency for memory-sized array
    printf("Main Memory Latency (Read): %.2f ns\n", measure_latency(memory_array, MEMORY_SIZE, 0));
    printf("Main Memory Latency (Write): %.2f ns\n", measure_latency(memory_array, MEMORY_SIZE, 1));
    
    // Free allocated memory
    free(cache_array);
    free(memory_array);
    
    return 0;
}
