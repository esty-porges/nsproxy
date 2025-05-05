#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>

// Forward declaration or include the header for MatrixServiceImpl
// Assuming MatrixServiceImpl is defined in matrix_service.cpp/h
#include "matrix_service.h" // You might need to create this header

using grpc::Server;
using grpc::ServerBuilder;

void RunServer() {
    std::string server_address("0.0.0.0:50051"); // Listen on all interfaces, port 50051
    MatrixServiceImpl matrix_service; // Instance of the service implementation

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&matrix_service);
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
