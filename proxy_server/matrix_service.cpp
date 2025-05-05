#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <grpcpp/grpcpp.h>

// Generated headers
#include "matrix_service.grpc.pb.h"
#include "matrix_service.h" // Include the header for the service implementation

using grpc::ServerContext;
using grpc::Status;
using matrix_service::LoadMatrixRequest;
using matrix_service::CreateZeroMatrixRequest;
using matrix_service::GetMatrixSizeRequest;
using matrix_service::MatrixInfoResponse;
using matrix_service::MatrixSizeResponse;

// Simple in-memory representation of a matrix (replace with your actual matrix library)
struct MatrixData {
    std::string name;
    int rows;
    int cols;
    // std::vector<double> data; // Add actual data storage if needed
};

// In-memory storage for matrices (replace with your actual storage mechanism)
std::map<std::string, MatrixData> matrix_storage;

// --- Implementation of MatrixServiceImpl methods --- 

grpc::Status MatrixServiceImpl::LoadMatrixFromFile(ServerContext* context, const matrix_service::LoadMatrixRequest* request,
                                                matrix_service::MatrixInfoResponse* response) {
    std::string matrix_name = request->matrix_name();
    std::string file_path = request->file_path();
    std::cout << "Received request to load matrix '" << matrix_name 
              << "' from file '" << file_path << "'" << std::endl;

    // --- Placeholder Implementation --- 
    // In a real implementation:
    // 1. Check if matrix_name already exists.
    // 2. Validate file_path.
    // 3. Open and parse the file (e.g., CSV, binary format).
    // 4. Determine dimensions (rows, cols) from the file.
    // 5. Store the matrix data (e.g., in matrix_storage or your system).
    // 6. Handle file not found, parsing errors, etc.

    // Simple placeholder: Assume loading is successful and deduce dummy dimensions.
    // You MUST replace this with actual file reading and dimension detection.
    int placeholder_rows = 3; // Replace with actual rows from file
    int placeholder_cols = 4; // Replace with actual cols from file

    matrix_storage[matrix_name] = {matrix_name, placeholder_rows, placeholder_cols};

    response->set_matrix_name(matrix_name);
    response->set_rows(placeholder_rows);
    response->set_cols(placeholder_cols);
    response->set_success(true);
    // --- End Placeholder --- 

    return Status::OK;
}

grpc::Status MatrixServiceImpl::CreateZeroMatrix(ServerContext* context, const matrix_service::CreateZeroMatrixRequest* request,
                                                matrix_service::MatrixInfoResponse* response) {
    std::string matrix_name = request->matrix_name();
    int rows = request->rows();
    int cols = request->cols();
    std::cout << "Received request to create zero matrix '" << matrix_name 
              << "' with size (" << rows << "x" << cols << ")" << std::endl;

    // --- Placeholder Implementation --- 
    // In a real implementation:
    // 1. Check if matrix_name already exists.
    // 2. Validate rows and cols (e.g., > 0).
    // 3. Allocate and store the zero matrix data.

    if (rows <= 0 || cols <= 0) {
        response->set_success(false);
        response->set_error_message("Matrix dimensions must be positive.");
        return Status(grpc::StatusCode::INVALID_ARGUMENT, "Matrix dimensions must be positive.");
    }

    if (matrix_storage.count(matrix_name)) {
         response->set_success(false);
         response->set_error_message("Matrix with this name already exists.");
         return Status(grpc::StatusCode::ALREADY_EXISTS, "Matrix already exists.");
    }

    matrix_storage[matrix_name] = {matrix_name, rows, cols};
    // Allocate actual zero data if needed here

    response->set_matrix_name(matrix_name);
    response->set_rows(rows);
    response->set_cols(cols);
    response->set_success(true);
    // --- End Placeholder --- 

    return Status::OK;
}

grpc::Status MatrixServiceImpl::GetMatrixSize(ServerContext* context, const matrix_service::GetMatrixSizeRequest* request,
                                            matrix_service::MatrixSizeResponse* response) {
    std::string matrix_name = request->matrix_name();
    std::cout << "Received request to get size for matrix '" << matrix_name << "'" << std::endl;

    auto it = matrix_storage.find(matrix_name);
    if (it != matrix_storage.end()) {
        response->set_rows(it->second.rows);
        response->set_cols(it->second.cols);
        response->set_success(true);
    } else {
        response->set_success(false);
        response->set_error_message("Matrix not found.");
        // Return OK status but indicate failure in the response payload
        // Alternatively, return grpc::StatusCode::NOT_FOUND
        // return Status(grpc::StatusCode::NOT_FOUND, "Matrix not found."); 
    }
    return Status::OK;
}
