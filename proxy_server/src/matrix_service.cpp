#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>

#include "matrix_operations.h"
#include "matrix_obj.h"
#include "matrix_store.h"
#include "matrix_service.grpc.pb.h"
#include "../include/matrix_service.h"
#include "../include/object_manager.h"

using namespace std;
using grpc::Status;

namespace {
matrix_service::MatrixStore& store = matrix_service::MatrixStore::instance();
}

Status MatrixServiceImpl::LoadMatrixFromFile(grpc::ServerContext* context, 
                                         const matrix_service::LoadMatrixRequest* request,
                                         matrix_service::MatrixInfoResponse* response) {
        const auto& matrix_name = request->matrix_name();
        const auto& file_path = request->file_path();
        
        cout << "Loading matrix '" << matrix_name << "' from '" << file_path << "'" << endl;

        // TODO: Get actual dimensions from file
        int rows = 3, cols = 4;

        if (!store.addMatrix(matrix_name, rows, cols)) {
            response->set_success(false);
            response->set_error_message("Failed to create matrix (may already exist)");
            return Status(grpc::StatusCode::ALREADY_EXISTS, "Matrix exists");
        }

        auto* matrix = store.getMatrix(matrix_name);
        if (!matrix) {
            response->set_success(false);
            response->set_error_message("Internal error: Matrix creation failed");
            return Status(grpc::StatusCode::INTERNAL, "Creation failed");
        }

        // TODO: Read matrix data from file into matrix->get_data()

        response->set_success(true);
        response->set_matrix_name(matrix_name);
        response->set_rows(matrix->get_rows());
        response->set_cols(matrix->get_cols());

    return Status::OK;
}

grpc::Status MatrixServiceImpl::CreateZeroMatrix(grpc::ServerContext* context, 
                                             const matrix_service::CreateZeroMatrixRequest* request,
                                             matrix_service::MatrixInfoResponse* response) {
        const auto& matrix_name = request->matrix_name();
        int rows = request->rows();
        int cols = request->cols();

        cout << "Creating zero matrix '" << matrix_name << "' (" << rows << "x" << cols << ")" << endl;

        if (rows <= 0 || cols <= 0) {
            response->set_success(false);
            response->set_error_message("Matrix dimensions must be positive");
            return Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid dimensions");
        }

        if (!store.addMatrix(matrix_name, rows, cols)) {
            response->set_success(false);
            response->set_error_message("Failed to create matrix (may already exist)");
            return Status(grpc::StatusCode::ALREADY_EXISTS, "Matrix exists");
        }

        response->set_success(true);
        response->set_matrix_name(matrix_name);
        response->set_rows(rows);
        response->set_cols(cols);

    return Status::OK;
}

grpc::Status MatrixServiceImpl::GetMatrixSize(grpc::ServerContext* context, 
                                          const matrix_service::GetMatrixSizeRequest* request,
                                          matrix_service::MatrixSizeResponse* response) {
        const auto& matrix_name = request->matrix_name();
        cout << "Getting size for matrix '" << matrix_name << "'" << endl;

        auto* matrix = store.getMatrix(matrix_name);
        if (!matrix) {
            response->set_success(false);
            response->set_error_message("Matrix not found");
            return Status(grpc::StatusCode::NOT_FOUND, "Not found");
        }

        response->set_success(true);
        response->set_rows(matrix->get_rows());
        response->set_cols(matrix->get_cols());
    return Status::OK;
}

grpc::Status MatrixServiceImpl::MultiplyMatrices(grpc::ServerContext* context,
                                             const matrix_service::MultiplyMatricesRequest* request,
                                             matrix_service::MatrixInfoResponse* response) {
        const auto& matrix_a_name = request->matrix_a_name();
        const auto& matrix_b_name = request->matrix_b_name();
        const auto& result_name = request->result_name();
        bool use_transpose = request->use_transpose();

        auto* matrix_a = store.getMatrix(matrix_a_name);
        auto* matrix_b = store.getMatrix(matrix_b_name);

        if (!matrix_a || !matrix_b) {
            response->set_success(false);
            response->set_error_message("Input matrix not found");
            return Status(grpc::StatusCode::NOT_FOUND, "Not found");
        }

        if (matrix_a->get_cols() != matrix_b->get_rows()) {
            response->set_success(false);
            response->set_error_message("Matrix dimensions mismatch");
            return Status(grpc::StatusCode::INVALID_ARGUMENT, "Dimensions mismatch");
        }

        if (!store.addMatrix(result_name, matrix_a->get_rows(), matrix_b->get_cols())) {
            response->set_success(false);
            response->set_error_message("Failed to create result matrix");
            return Status(grpc::StatusCode::ALREADY_EXISTS, "Result exists");
        }

        auto* result = store.getMatrix(result_name);

        if (!result) {
            response->set_success(false);
            response->set_error_message("Internal error: Result creation failed");
            return Status(grpc::StatusCode::INTERNAL, "Creation failed");
        }

        if (use_transpose) {
            auto B_T = make_unique<float[]>(matrix_b->get_rows() * matrix_b->get_cols());
            matrix_ops::mult_transpose(
                matrix_a->get_data(),
                matrix_b->get_data(),
                B_T.get(),
                result->get_data(),
                matrix_a->get_rows()
            );
        } else {
            matrix_ops::mult_omp(
                matrix_a->get_data(),
                matrix_b->get_data(),
                result->get_data(),
                matrix_a->get_rows()
            );
        }

        response->set_success(true);
        response->set_matrix_name(result_name);
        response->set_rows(result->get_rows());
        response->set_cols(result->get_cols());

    return Status::OK;
}

// List objects implementation
grpc::Status MatrixServiceImpl::ListObjects(grpc::ServerContext* context,
                                          const matrix_service::ListObjectsRequest* request,
                                          matrix_service::ListObjectsResponse* response) {
        cout << "Listing objects" << endl;

        for (const auto& name : store.getRegisteredMatrices()) {
            auto* info = response->add_objects();
            info->set_name(name);
            info->set_type("Matrix");
        }

        return Status::OK;
}
