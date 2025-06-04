#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <nlohmann/json.hpp>

// Forward declaration or include the header for MatrixServiceImpl
// Assuming MatrixServiceImpl is defined in matrix_service.cpp/h
#include "../include/matrix_service.h" // You might need to create this header

using grpc::Server;
using grpc::ServerBuilder;

void RunServer() {
    // Read port from config.json
    int port = 50051; // default port
    std::string config_path = std::string(getenv("PWD")) + "/config.json";
    std::ifstream config_file(config_path);
    std::cout << "Looking for config file at: " << config_path << std::endl;
    if (config_file.is_open()) {
        try {
            nlohmann::json config;
            config_file >> config;
            if (config.contains("port")) {
                port = config["port"].get<int>();
            }
        } catch (const std::exception& e) {
            std::cerr << "Error reading config file: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Using default port " << port << " (config.json not found)" << std::endl;
    }

    std::string server_address = "0.0.0.0:" + std::to_string(port);
    MatrixServiceImpl matrix_service; // Instance of the service implementation

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&matrix_service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    if (!server) {
        std::cerr << "Failed to start server on " << server_address << std::endl;
        return;
    }
    
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
