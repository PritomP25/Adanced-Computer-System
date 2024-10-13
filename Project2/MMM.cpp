#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <immintrin.h> // For SIMD intrinsics

// Global variables for configuration
bool multi_threads = false;
bool simd = false;
bool cache_opt = false;
bool all_three = false;

int size = 1000; //default matrix size (size x size)
float sparsity = 0.0; //default sparsity
int num_threads = 1;  // Default to single thread
int blockSize = 32; // Block size for cache optimization

std::mutex mtx; // Mutex for thread safety

// Function to create the matrix based on sparsity
std::vector<std::vector<float>> createMatrix(int size, float sparsity) {
    std::vector<std::vector<float>> matrix(size, std::vector<float>(size, 0.0)); // Initialize matrix with 0s
    
    srand(time(0)); // Seed for random number generation
    
    // If sparsity is 0, create a dense matrix
    if (sparsity == 0.0) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                matrix[i][j] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // Cast RAND_MAX to float
            }
        }
    } 
    // If sparsity > 0, create a sparse matrix
    else {
        int non_zero_elements = static_cast<int>((1.0 - sparsity) * size * size); // Number of non-zero elements
        while (non_zero_elements > 0) {
            int i = rand() % size;
            int j = rand() % size;
            // If the chosen position is still zero, fill it with a random value
            if (matrix[i][j] == 0.0) {
                matrix[i][j] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // Cast RAND_MAX to float
                non_zero_elements--;
            }
        }
    }
    
    return matrix;
}

// Function to multiply a portion of matrices A and B, and store the result in matrix C
void multiply_matrices_chunk(const std::vector<std::vector<float>>& A, 
                              const std::vector<std::vector<float>>& B, 
                              std::vector<std::vector<float>>& C, 
                              int start_row, int end_row) {
    int n = A.size();

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < n; j++) {
            // Compute the dot product of row i of A and column j of B
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// SIMD optimized function for multiplying matrices
void multiply_matrices_simd(const std::vector<std::vector<float>>& A, 
                              const std::vector<std::vector<float>>& B, 
                              std::vector<std::vector<float>>& C) {
    int n = A.size();
    C = std::vector<std::vector<float>>(n, std::vector<float>(n, 0.0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            __m256 sum = _mm256_setzero_ps(); // Set sum to 0

            for (int k = 0; k < n; k += 8) { // Process 8 elements at a time
                __m256 a_values = _mm256_loadu_ps(&A[i][k]); // Load 8 elements from row i of A
                __m256 b_values = _mm256_loadu_ps(&B[k][j]); // Load 8 elements from column j of B
                sum = _mm256_fmadd_ps(a_values, b_values, sum); // Fused multiply-add
            }

            // Horizontal sum to compute the final value for C[i][j]
            float result[8]; // To store the results from the vector
            _mm256_storeu_ps(result, sum); // Store the intermediate results
            C[i][j] = result[0] + result[1] + result[2] + result[3] + result[4] + result[5] + result[6] + result[7];
        }
    }
}

// Function to multiply matrices A and B, and store the result in matrix C with cache optimization
void multiply_matrices_cache_opt(const std::vector<std::vector<float>>& A, 
                                  const std::vector<std::vector<float>>& B, 
                                  std::vector<std::vector<float>>& C, 
                                  int blockSize) {
    int n = A.size();

    // Initialize the result matrix C with zeros
    C = std::vector<std::vector<float>>(n, std::vector<float>(n, 0.0));

    // Matrix multiplication with cache optimization (blocking)
    for (int i = 0; i < n; i += blockSize) {
        for (int j = 0; j < n; j += blockSize) {
            for (int k = 0; k < n; k += blockSize) {
                // Multiply the blocks
                for (int ii = i; ii < std::min(i + blockSize, n); ++ii) {
                    for (int jj = j; jj < std::min(j + blockSize, n); ++jj) {
                        for (int kk = k; kk < std::min(k + blockSize, n); ++kk) {
                            C[ii][jj] += A[ii][kk] * B[kk][jj];
                        }
                    }
                }
            }
        }
    }
}


// Function to multiply matrices A and B, and store the result in matrix C using all three optimizations
void all_opt_func(const std::vector<std::vector<float>>& A, 
                               const std::vector<std::vector<float>>& B, 
                               std::vector<std::vector<float>>& C, 
                               int blockSize) {
    int n = A.size();

    // Initialize the result matrix C with zeros
    C = std::vector<std::vector<float>>(n, std::vector<float>(n, 0.0));

    // Parallelize the outer loops using OpenMP
    #pragma omp parallel for num_threads(num_threads) collapse(2) 
    for (int i = 0; i < n; i += blockSize) {
        for (int j = 0; j < n; j += blockSize) {
            for (int k = 0; k < n; k += blockSize) {
                // Multiply the blocks with SIMD inside
                for (int ii = i; ii < std::min(i + blockSize, n); ++ii) {
                    for (int jj = j; jj < std::min(j + blockSize, n); ++jj) {
                        __m256 sum_vec = _mm256_setzero_ps(); // Initialize the sum vector for SIMD
                        for (int kk = k; kk < std::min(k + blockSize, n); kk += 8) {
                            // Load 8 floats from A and B at a time
                            __m256 a_values = _mm256_loadu_ps(&A[ii][kk]);
                            __m256 b_values = _mm256_loadu_ps(&B[kk][jj]);
                            sum_vec = _mm256_fmadd_ps(a_values, b_values, sum_vec); // Fused multiply-add
                        }
                        // Add the sum of the SIMD vector elements to C
                        float sum[8];
                        _mm256_storeu_ps(sum, sum_vec);  // Store the vector result
                        C[ii][jj] += sum[0] + sum[1] + sum[2] + sum[3] + sum[4] + sum[5] + sum[6] + sum[7];
                    }
                }
            }
        }
    }
}


// Function to multiply matrices A and B, and store the result in matrix C
void multiply_matrices(const std::vector<std::vector<float>>& A, 
                       const std::vector<std::vector<float>>& B, 
                       std::vector<std::vector<float>>& C) {
    
    if (simd) {
        multiply_matrices_simd(A, B, C);
    }else{
        int n = A.size();

        // Initialize the result matrix C with zeros
        C = std::vector<std::vector<float>>(n, std::vector<float>(n, 0.0));

        if (multi_threads) {
            std::vector<std::thread> threads;
            int rows_per_thread = n / num_threads;
            
            for (int t = 0; t < num_threads; t++) {
                int start_row = t * rows_per_thread;
                int end_row = (t == num_threads - 1) ? n : start_row + rows_per_thread;
                threads.emplace_back(multiply_matrices_chunk, std::cref(A), std::cref(B), std::ref(C), start_row, end_row);
            }
            
            // Join threads
            for (auto& th : threads) {
                if (th.joinable()) {
                    th.join();
                }
            }
        } else {
            // Single-threaded multiplication (no optimiation)
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    for (int k = 0; k < n; k++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // Loop through the arguments
        for (int i = 1; i < argc; i++) {
            // Check for the mandatory argument --size
            if (strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
                size = atoi(argv[++i]);
            }
            //add-on argument --sparsity, which takes an float value in pecetange
            else if (strcmp(argv[i], "--sparsity") == 0 && i + 1 < argc) {
                sparsity = atof(argv[++i]) / 100.0;
            }
            //choose one of the following optimizations below
            //--mult_threads, which takes an integer value
            else if (strcmp(argv[i], "--mult_threads") == 0 && i + 1 < argc) {
                num_threads = atoi(argv[++i]);
                multi_threads = true;
            }
            //argument --simd
            else if (strcmp(argv[i], "--simd") == 0) {
                simd = true;
            }
            //argument --cache
            else if (strcmp(argv[i], "--cache") == 0) {
                cache_opt = true;
            }
            //argument --all
            else if (strcmp(argv[i], "--all") == 0) {
                all_three = true;
            }
        }

        // Validate that the mandatory --size argument is provided
        if (size == 0) {
            std::cerr << "Error: --size is a mandatory argument.\n";
            return 1;
        }

        // Output the provided arguments
        std::cout << "Matrix Size: " << size << "x" << size << std::endl;
        std::cout << "Matrix Sparsity: " << sparsity * 100 << "%" << std::endl;

        if (all_three) {
            std::cout << "All optimizations enabled (multi-threading, SIMD, cache minimization).\n";
        } else {
            if (multi_threads) {
                std::cout << "Multi-threading enabled with " << num_threads << " threads.\n";
            } else if (simd) {
                std::cout << "SIMD optimization enabled.\n";
            } else if (cache_opt) {
                std::cout << "Cache miss minimization enabled.\n";
            }
        }

    } else {
        std::cerr << "Error: Not enough arguments. Please provide --size as a mandatory argument.\n";
        return 1;
    }

    // Create the matrix
    std::vector<std::vector<float>> A = createMatrix(size, sparsity);

    std::vector<std::vector<float>> B = createMatrix(size, sparsity);
    
    //uncomment this block if you are planning to do a Dense-Sparsity matrix multiplication and manunally changes the size and sparsity, otherwise uncomment above and comment this matrix
    //std::vector<std::vector<float>> B = createMatrix(size, 10.0);

    std::vector<std::vector<float>> C;

    // Measure time
    auto start = std::chrono::high_resolution_clock::now();

    if(all_three){
        num_threads = 4;
        all_opt_func(A,B,C,blockSize);
    }else if (cache_opt) {
        // Call the matrix multiplication function with cache optimization
        multiply_matrices_cache_opt(A, B, C, blockSize);
    } else {
        // Call the regular matrix multiplication function
        multiply_matrices(A, B, C);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << "Matrix multiplication completed in " << elapsed.count() << " seconds." << std::endl;

    return 0;
}
