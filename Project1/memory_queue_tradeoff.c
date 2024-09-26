#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MEMORY_SIZE 64 * 1024 * 1024  // 64MB of memory for testing

// Structure to pass data to thread function
typedef struct {
    int *array;
    size_t size;
    int is_write;
} thread_data_t;

// Function to simulate memory access (read or write)
void *memory_access(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    volatile int temp;
    
    for (size_t i = 0; i < data->size; i += 64) {  // Access every 64 bytes (cache line)
        if (data->is_write) {
            data->array[i] = i;  // Write to memory
        } else {
            temp = data->array[i];  // Read from memory
        }
    }
    
    return NULL;
}

// Function to measure latency and throughput based on queue depth
void measure_tradeoff(int *array, size_t size, int queue_depth, int is_write) {
    struct timespec start, end;
    pthread_t threads[queue_depth];
    thread_data_t thread_data[queue_depth];
    
    // Prepare thread data for all threads
    for (int i = 0; i < queue_depth; i++) {
        thread_data[i].array = array;
        thread_data[i].size = size;
        thread_data[i].is_write = is_write;
    }
    
    // Get start time
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Create and start threads to simulate parallel memory access
    for (int i = 0; i < queue_depth; i++) {
        pthread_create(&threads[i], NULL, memory_access, &thread_data[i]);
    }
    
    // Wait for all threads to finish
    for (int i = 0; i < queue_depth; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Get end time
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calculate total time in seconds
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    
    // Calculate latency per operation
    double latency_per_access = (time_taken * 1e9) / (size / 64 * queue_depth);  // ns per access
    
    // Calculate throughput in bytes per second
    double throughput = (size / time_taken) * queue_depth;
    
    // Print results
    printf("Queue Depth: %d, Latency: %.2f ns, Throughput: %.2f MB/s\n",
           queue_depth, latency_per_access, throughput / (1024 * 1024));
}

int main() {
    // Allocate memory for the test
    int *memory_array = (int *)malloc(MEMORY_SIZE * sizeof(int));
    if (memory_array == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Test different queue depths
    int queue_depths[] = {1, 2, 4, 8, 16, 32};
    
    // Measure read latency and throughput for different queue depths
    printf("Read Latency vs Throughput (Queue Depths):\n");
    for (int i = 0; i < 6; i++) {
        measure_tradeoff(memory_array, MEMORY_SIZE, queue_depths[i], 0);
    }
    
    // Measure write latency and throughput for different queue depths
    printf("\nWrite Latency vs Throughput (Queue Depths):\n");
    for (int i = 0; i < 6; i++) {
        measure_tradeoff(memory_array, MEMORY_SIZE, queue_depths[i], 1);
    }
    
    // Free memory
    free(memory_array);
    
    return 0;
}
