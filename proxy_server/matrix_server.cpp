#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <grpcpp/grpcpp.h>

// Generated headers
#include "matrix_service.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using matrix_service::MatrixService;
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

// Logic and data behind the server's behavior.
class MatrixServiceImpl final : public MatrixService::Service {

    Status LoadMatrixFromFile(ServerContext* context, const LoadMatrixRequest* request,
                              MatrixInfoResponse* response) override {
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

    Status CreateZeroMatrix(ServerContext* context, const CreateZeroMatrixRequest* request,
                            MatrixInfoResponse* response) override {
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

    Status GetMatrixSize(ServerContext* context, const GetMatrixSizeRequest* request,
                         MatrixSizeResponse* response) override {
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
};

void RunServer() {
    std::string server_address("0.0.0.0:50051"); // Listen on all interfaces, port 50051
    MatrixServiceImpl service;

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
