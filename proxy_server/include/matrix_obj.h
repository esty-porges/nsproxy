#pragma once

#include <string>
#include <memory>
#include <algorithm>

namespace matrix_service {

class MatrixObj {

private:
    std::string name;
    int rows;
    int cols;
    std::unique_ptr<float[]> data;

public:
    MatrixObj(const std::string& n, int r, int c);

    // Get raw pointer for matrix operations
    float* get_data() { return data.get(); }
    const float* get_data() const { return data.get(); }

    // Getters
    const std::string& get_name() const { return name; }
    int get_rows() const { return rows; }
    int get_cols() const { return cols; }

     // Prevent copying
    MatrixObj(const MatrixObj&) = delete;
    MatrixObj& operator=(const MatrixObj&) = delete;

    MatrixObj(MatrixObj&&) = default;
    MatrixObj& operator=(MatrixObj&&) = default;
};

} // namespace matrix_service
