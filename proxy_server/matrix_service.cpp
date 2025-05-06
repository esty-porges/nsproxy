#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <grpcpp/grpcpp.h>

// Generated headers
#include "matrix_service.grpc.pb.h"
#include "matrix_service.h" // Include the header for the service implementation
#include "object_manager.h" // Include the ObjectManager header

using grpc::ServerContext;
using grpc::Status;
using matrix_service::LoadMatrixRequest;
using matrix_service::CreateZeroMatrixRequest;
using matrix_service::GetMatrixSizeRequest;
using matrix_service::MatrixInfoResponse;
using matrix_service::MatrixSizeResponse;
using matrix_service::ListObjectsRequest;   // Add new types
using matrix_service::ListObjectsResponse;  // Add new types
using matrix_service::ObjectInfo;           // Add new types

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

    // Check if object is already registered using ObjectManager
    if (ObjectManager::instance().is_registered(matrix_name)) {
        response->set_success(false);
        response->set_error_message("Object with this name already exists.");
        return Status(grpc::StatusCode::ALREADY_EXISTS, "Object already exists.");
    }

    // --- Placeholder Implementation ---
    // In a real implementation:
    // 1. Validate file_path.
    // 2. Open and parse the file (e.g., CSV, binary format).
    // 3. Determine dimensions (rows, cols) from the file.
    // 4. Store the matrix data (e.g., in matrix_storage or your system).
    // 5. Handle file not found, parsing errors, etc.

    // Simple placeholder: Assume loading is successful and deduce dummy dimensions.
    // You MUST replace this with actual file reading and dimension detection.
    int placeholder_rows = 3; // Replace with actual rows from file
    int placeholder_cols = 4; // Replace with actual cols from file

    matrix_storage[matrix_name] = {matrix_name, placeholder_rows, placeholder_cols};

    response->set_matrix_name(matrix_name);
    response->set_rows(placeholder_rows);
    response->set_cols(placeholder_cols);
    response->set_success(true); // Assume success for placeholder
    // --- End Placeholder ---

    // Register the object name on successful load
    if (response->success()) {
         if (!ObjectManager::instance().register_object(matrix_name)) {
             // This should ideally not happen if the initial check passed, but handle defensively
             response->set_success(false);
             response->set_error_message("Failed to register object after loading (concurrent modification?).");
             // Clean up the matrix data that was just added
             matrix_storage.erase(matrix_name);
             return Status(grpc::StatusCode::INTERNAL, "Failed to register object.");
         }
         std::cout << "Registered object: " << matrix_name << std::endl;
    } else {
        // If loading failed (e.g., file not found in real implementation), ensure no partial data remains
        matrix_storage.erase(matrix_name);
    }

    return Status::OK;
}

grpc::Status MatrixServiceImpl::CreateZeroMatrix(ServerContext* context, const matrix_service::CreateZeroMatrixRequest* request,
                                                matrix_service::MatrixInfoResponse* response) {
    std::string matrix_name = request->matrix_name();
    int rows = request->rows();
    int cols = request->cols();
    std::cout << "Received request to create zero matrix '" << matrix_name
              << "' with size (" << rows << "x" << cols << ")" << std::endl;

    // Check if object is already registered using ObjectManager
    if (ObjectManager::instance().is_registered(matrix_name)) {
        response->set_success(false);
        response->set_error_message("Object with this name already exists.");
        return Status(grpc::StatusCode::ALREADY_EXISTS, "Object already exists.");
    }

    // --- Placeholder Implementation ---
    // In a real implementation:
    // 1. Validate rows and cols (e.g., > 0).
    // 2. Allocate and store the zero matrix data.

    if (rows <= 0 || cols <= 0) {
        response->set_success(false);
        response->set_error_message("Matrix dimensions must be positive.");
        return Status(grpc::StatusCode::INVALID_ARGUMENT, "Matrix dimensions must be positive.");
    }

    // Removed the check here: if (matrix_storage.count(matrix_name)) { ... }
    // as ObjectManager handles the existence check now.

    matrix_storage[matrix_name] = {matrix_name, rows, cols};
    // Allocate actual zero data if needed here

    response->set_matrix_name(matrix_name);
    response->set_rows(rows);
    response->set_cols(cols);
    response->set_success(true); // Assume success for placeholder
    // --- End Placeholder ---

    // Register the object name on successful creation
    if (response->success()) {
        if (!ObjectManager::instance().register_object(matrix_name)) {
            // This should ideally not happen if the initial check passed, but handle defensively
            response->set_success(false);
            response->set_error_message("Failed to register object after creation (concurrent modification?).");
            // Clean up the matrix data that was just added
            matrix_storage.erase(matrix_name);
            return Status(grpc::StatusCode::INTERNAL, "Failed to register object.");
        }
        std::cout << "Registered object: " << matrix_name << std::endl;
    } else {
         // If creation failed (e.g., invalid dimensions), ensure no partial data remains
        matrix_storage.erase(matrix_name);
    }

    return Status::OK;
}

grpc::Status MatrixServiceImpl::GetMatrixSize(ServerContext* context, const matrix_service::GetMatrixSizeRequest* request,
                                            matrix_service::MatrixSizeResponse* response) {
    std::string matrix_name = request->matrix_name();
    std::cout << "Received request to get size for matrix '" << matrix_name << "'" << std::endl;

    // Check if object is registered before trying to access storage
    if (!ObjectManager::instance().is_registered(matrix_name)) {
         response->set_success(false);
         response->set_error_message("Object not found.");
         return Status(grpc::StatusCode::NOT_FOUND, "Object not found.");
    }

    auto it = matrix_storage.find(matrix_name);
    if (it != matrix_storage.end()) {
        response->set_rows(it->second.rows);
        response->set_cols(it->second.cols);
        response->set_success(true);
    } else {
        // This case should ideally not be reached if ObjectManager is consistent with matrix_storage
        response->set_success(false);
        response->set_error_message("Matrix data not found despite being registered.");
        // Return OK status but indicate failure in the response payload
        // Or consider returning INTERNAL error status
        // return Status(grpc::StatusCode::INTERNAL, "Inconsistent state: registered but no data.");
    }
    return Status::OK;
}

// New method implementation
grpc::Status MatrixServiceImpl::ListObjects(ServerContext* context, const ListObjectsRequest* request,
                                            ListObjectsResponse* response) {
    std::cout << "Received request to list objects." << std::endl;
    // Get names directly from ObjectManager
    auto object_names = ObjectManager::instance().get_registered_objects();

    for (const auto& name : object_names) {
        ObjectInfo* info = response->add_objects(); // Add a new ObjectInfo to the response
        info->set_name(name);
        // For now, assume all registered objects are matrices
        // If types were stored, retrieve and convert type here
        info->set_type("Matrix");
    }

    return Status::OK;
}
