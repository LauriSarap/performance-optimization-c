#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <unistd.h>

/* 
TODO:
1. Right now cache unfriendly access pattern
    * Memory is stored in cache row-wise in C
    * Current matrix multiplication traverses matrix B column-wise
    * Hence no use of cache
2. Tiled matrix multiplication
    * Matrix should be divided into blocks that fit into CPU cache
    * Reduces cache misses
    * Optimal block size is dependent on cache CPU cache size
3. SIMD instructions
    * Single instruction multiple 
4. Parallelization

*/

#define N 2048 // Matrix size
#define MAX_RANDOM 10 // Maximum random number (not included)

void initializeRandomMatrix(int **mat) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            mat[i][j] = rand() % MAX_RANDOM;
        }
    }
}

void multiplyMatrices(int **A, int **B, int **C) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            C[i][j] = 0;
            for(int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Helper function to allocate a 2D array on the heap
int ** allocateMatrix() {
    // Array of N pointers
    int **matrix = (int**)malloc(N * sizeof(int*));

    // N arrays of N integers
    for (int i = 0; i < N; i++) {
        matrix[i] = (int*)malloc(N * sizeof(int));
    }
    return matrix;
}

void freeMatrix(int **matrix) {
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);
}


int main() {

    struct rusage r_usage;
    
    size_t total_bytes = (size_t)N * N * sizeof(int) * 3;
    printf("Allocating 3 matrices of size %dx%d\n", N, N);
    printf("Total memory needed: %.2f MB\n", total_bytes / (1024.0 * 1024.0));

    // Seed for the random number generator
    srand(time(NULL));

    int **A = allocateMatrix();
    int **B = allocateMatrix();
    int **C = allocateMatrix();

    initializeRandomMatrix(A);
    initializeRandomMatrix(B);

    getrusage(RUSAGE_SELF, &r_usage);
    printf("Memory usage before multiplication: %.2f MB\n", 
           r_usage.ru_maxrss / 1024.0);
    
    clock_t start = clock();
    multiplyMatrices(A, B, C);
    clock_t end = clock();

    getrusage(RUSAGE_SELF, &r_usage);
    printf("Memory usage after multiplication: %.2f MB\n", 
           r_usage.ru_maxrss / 1024.0);

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Matrix multiplication took: %f seconds\n", time_spent);
    printf("CPU time used: User %.2f sec, System %.2f sec\n",
        r_usage.ru_utime.tv_sec + r_usage.ru_utime.tv_usec / 1000000.0,
        r_usage.ru_stime.tv_sec + r_usage.ru_stime.tv_usec / 1000000.0);
    
    freeMatrix(A);
    freeMatrix(B);
    freeMatrix(C);
    
    return 0;
}