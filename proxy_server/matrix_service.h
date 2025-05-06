#ifndef MATRIX_SERVICE_H
#define MATRIX_SERVICE_H

#include <grpcpp/grpcpp.h>
#include "matrix_service.grpc.pb.h" // Include generated gRPC header

// Declare the service implementation class
class MatrixServiceImpl final : public matrix_service::MatrixService::Service {
public:
    // Method declarations only
    grpc::Status LoadMatrixFromFile(grpc::ServerContext* context, const matrix_service::LoadMatrixRequest* request,
                                    matrix_service::MatrixInfoResponse* response) override;

    grpc::Status CreateZeroMatrix(grpc::ServerContext* context, const matrix_service::CreateZeroMatrixRequest* request,
                                  matrix_service::MatrixInfoResponse* response) override;

    grpc::Status GetMatrixSize(grpc::ServerContext* context, const matrix_service::GetMatrixSizeRequest* request,
                               matrix_service::MatrixSizeResponse* response) override;

    // New method declaration
    grpc::Status ListObjects(grpc::ServerContext* context, const matrix_service::ListObjectsRequest* request,
                             matrix_service::ListObjectsResponse* response) override;
};

#endif // MATRIX_SERVICE_H
