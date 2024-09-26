#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SMALL_SIZE 512   // Small matrix size fits in TLB (less likely to miss)
#define LARGE_SIZE 8192  // Large matrix size more likely to cause TLB misses

// Function to initialize matrices with random values
void initialize_matrix(double *matrix, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i * size + j] = (double)rand() / RAND_MAX;
        }
    }
}

// Matrix multiplication function (C = A * B)
void matrix_multiply(double *A, double *B, double *C, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            C[i * size + j] = 0;
            for (int k = 0; k < size; ++k) {
                C[i * size + j] += A[i * size + k] * B[k * size + j];
            }
        }
    }
}

// Function that touches the memory in a TLB-unfriendly way
void induce_tlb_misses(double *matrix, int size, int step) {
    for (int i = 0; i < size * size; i += step) {
        matrix[i] = (double)rand() / RAND_MAX;
    }
}

int main() {
    srand(time(NULL));

    // Allocate memory for small matrices
    double *A_small = (double *)malloc(SMALL_SIZE * SMALL_SIZE * sizeof(double));
    double *B_small = (double *)malloc(SMALL_SIZE * SMALL_SIZE * sizeof(double));
    double *C_small = (double *)malloc(SMALL_SIZE * SMALL_SIZE * sizeof(double));

    // Allocate memory for large matrices
    double *A_large = (double *)malloc(LARGE_SIZE * LARGE_SIZE * sizeof(double));
    double *B_large = (double *)malloc(LARGE_SIZE * LARGE_SIZE * sizeof(double));
    double *C_large = (double *)malloc(LARGE_SIZE * LARGE_SIZE * sizeof(double));

    // Initialize matrices with random values
    initialize_matrix(A_small, SMALL_SIZE);
    initialize_matrix(B_small, SMALL_SIZE);
    initialize_matrix(A_large, LARGE_SIZE);
    initialize_matrix(B_large, LARGE_SIZE);

    // Induce TLB misses on the large matrices
    induce_tlb_misses(A_large, LARGE_SIZE, getpagesize() / sizeof(double));
    induce_tlb_misses(B_large, LARGE_SIZE, getpagesize() / sizeof(double));

    // Measure performance for small matrix
    clock_t start = clock();
    matrix_multiply(A_small, B_small, C_small, SMALL_SIZE);
    clock_t end = clock();
    double small_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for small matrix (less TLB misses): %f seconds\n", small_time);

    // Measure performance for large matrix (more TLB misses)
    start = clock();
    matrix_multiply(A_large, B_large, C_large, LARGE_SIZE);
    end = clock();
    double large_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for large matrix (more TLB misses): %f seconds\n", large_time);

    // Free allocated memory
    free(A_small);
    free(B_small);
    free(C_small);
    free(A_large);
    free(B_large);
    free(C_large);

    return 0;
}
