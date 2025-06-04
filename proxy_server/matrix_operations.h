#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

namespace matrix_ops {

// Matrix multiplication functions using raw pointers
void transposeMatrix(const float* B, float* B_T, int N);
void mult_transpose(const float* A, const float* B, float* B_T, float* C, int N);
void mult_omp(const float* A, const float* B, float* C, int N);

} // namespace matrix_ops

#endif // MATRIX_OPERATIONS_H
