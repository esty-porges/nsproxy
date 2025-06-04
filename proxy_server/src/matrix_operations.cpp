#include "../include/matrix_operations.h"
#include <omp.h>

namespace matrix_ops {

void transposeMatrix(const float* B, float* B_T, int N) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            B_T[j * N + i] = B[i * N + j];
}

void mult_transpose(const float* A, const float* B, float* B_T, float* C, int N) {
    transposeMatrix(B, B_T, N);

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            float sum = 0;
            for (int k = 0; k < N; k++)
                sum += A[(i * N) + k] * B_T[(k * N) + j];  
            C[(i * N) + j] = sum;
        }
}

void mult_omp(const float* A, const float* B, float* C, int N) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float sum = 0;
            for (int k = 0; k < N; k++)
                sum += A[(i * N) + k] * B[(k * N) + j];
            C[(i * N) + j] = sum;
        }
    }
}
}
