cmake_minimum_required(VERSION 3.15) # Recommended minimum for better Conan integration
project(MatrixServer CXX)

# --- Compiler Setup ---
# Use clang++
set(CMAKE_CXX_COMPILER "clang++")
# Add C++ standard (optional, but good practice)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Find Packages ---
# Find gRPC using CMake's find_package, request grpc++ component
find_package(gRPC CONFIG REQUIRED COMPONENTS grpc++)

# Find Protobuf first, as gRPC's CMake files might depend on it
find_package(protobuf REQUIRED)


# --- Find Python from Virtual Environment ---
set(VENV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/venv)
if(EXISTS ${VENV_DIR} AND IS_DIRECTORY ${VENV_DIR})
    # Set Python3_ROOT_DIR to the virtual environment directory
    set(Python3_ROOT_DIR ${VENV_DIR})
    message(STATUS "Found Python virtual environment: ${VENV_DIR}")
    message(STATUS "Setting Python3_ROOT_DIR to: ${Python3_ROOT_DIR}")

    # Try finding the interpreter executable explicitly first (optional but good check)
    find_program(Python3_EXECUTABLE NAMES python python3 HINTS ${VENV_DIR}/bin NO_DEFAULT_PATH)
    if(NOT Python3_EXECUTABLE)
         message(FATAL_ERROR "Could not find Python executable in ${VENV_DIR}/bin.")
    endif()
    message(STATUS "Using Python interpreter: ${Python3_EXECUTABLE}")

else()
    message(FATAL_ERROR "Python virtual environment directory venv not found in ${CMAKE_CURRENT_SOURCE_DIR}. Please create it first (e.g., python3 -m venv venv).")
endif()

# Find Python components using the specified root directory
find_package(Python3 COMPONENTS Interpreter REQUIRED) # Rely on Python3_ROOT_DIR

# Check if found
if(NOT Python3_FOUND)
    message(FATAL_ERROR "CMake could not find Python3 using Python3_ROOT_DIR=${Python3_ROOT_DIR}. Ensure Python development libraries are installed in the venv or system.")
endif()

# --- Protobuf Code Generation ---
# Define the proto file
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/matrix_service.proto) # Updated path

# Define output directory for generated files
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

# Ensure Protobuf package is found before using its functions
# (find_package(protobuf REQUIRED) is already called earlier)

# Generate base Protobuf C++ sources and headers
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    PROTOS ${PROTO_FILE}
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/proto # Updated path
    OUT_DIR ${PROTO_GEN_DIR}
    PROTOC_TARGET protobuf::protoc # Use Conan's protoc
)

# --- Manually Generate gRPC C++ Code ---
# Define expected output files for gRPC
set(GRPC_CPP_HDR ${PROTO_GEN_DIR}/matrix_service.grpc.pb.h)
set(GRPC_CPP_SRC ${PROTO_GEN_DIR}/matrix_service.grpc.pb.cc)
set(GRPC_SRCS ${GRPC_CPP_SRC}) # Keep variable names consistent if needed later
set(GRPC_HDRS ${GRPC_CPP_HDR})

# Find the gRPC C++ plugin executable provided by the gRPC package
get_target_property(GRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)
if(NOT GRPC_CPP_PLUGIN_EXECUTABLE)
    message(FATAL_ERROR "Could not find gRPC C++ plugin location (gRPC::grpc_cpp_plugin target).")
endif()

# Custom command to generate gRPC C++ sources
add_custom_command(
    OUTPUT ${GRPC_CPP_HDR} ${GRPC_CPP_SRC}
    COMMAND $<TARGET_FILE:protobuf::protoc> # Use protoc executable from Conan target
            -I ${CMAKE_CURRENT_SOURCE_DIR}/proto # Updated path
            --cpp_out=${PROTO_GEN_DIR}
            --grpc_out=${PROTO_GEN_DIR}
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
            ${PROTO_FILE}
    DEPENDS ${PROTO_FILE} protobuf::protoc gRPC::grpc_cpp_plugin
    COMMENT "Generating gRPC C++ code from ${PROTO_FILE}"
    VERBATIM
)

# Custom target to ensure gRPC C++ code generation happens
add_custom_target(generate_grpc_cpp ALL
    DEPENDS ${GRPC_CPP_HDR} ${GRPC_CPP_SRC}
)

# --- Generate Python gRPC Code ---
# Find the Python gRPC tools protoc plugin
# This assumes grpcio-tools is installed and python3 -m grpc_tools.protoc works
find_program(GRPC_PYTHON_PROTOC_COMMAND NAMES "python3" REQUIRED)
set(GRPC_PYTHON_PROTOC_MODULE "-mgrpc_tools.protoc")

# Define output directory for Python generated files
set(PYTHON_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python_client) # Updated path

# Custom command to generate Python sources
add_custom_command(
    OUTPUT ${PYTHON_GEN_DIR}/matrix_service_pb2.py ${PYTHON_GEN_DIR}/matrix_service_pb2_grpc.py # Updated paths
    COMMAND ${Python3_EXECUTABLE} ${GRPC_PYTHON_PROTOC_MODULE} # Use the found Python executable
            -I${CMAKE_CURRENT_SOURCE_DIR}/proto # Updated path
            --python_out=${PYTHON_GEN_DIR} 
            --grpc_python_out=${PYTHON_GEN_DIR} 
            ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating Python gRPC code from ${PROTO_FILE}"
    VERBATIM
)

# Custom target to trigger Python code generation (optional, but good practice)
add_custom_target(generate_python_grpc ALL
    DEPENDS ${PYTHON_GEN_DIR}/matrix_service_pb2.py ${PYTHON_GEN_DIR}/matrix_service_pb2_grpc.py # Updated paths
)

# --- Build the Server Executable ---
# Add executable and include generated sources directly
add_executable(matrix_server proxy_server/matrix_server.cpp # Updated path with .cpp extension
    ${PROTO_SRCS} ${PROTO_HDRS}
    ${GRPC_SRCS} ${GRPC_HDRS} # Include manually generated gRPC sources/headers
)

# Add include directories
target_include_directories(matrix_server PUBLIC
    ${PROTO_GEN_DIR} # For generated headers
    # Conan handles Protobuf/gRPC include dirs via targets
)

# Link libraries
# Use the target names provided by Conan
target_link_libraries(matrix_server PUBLIC
    protobuf::libprotobuf   # Protobuf runtime target from Conan
    gRPC::grpc++            # gRPC C++ library from Conan
    OpenSSL::SSL            # Explicitly link OpenSSL SSL
    OpenSSL::Crypto         # Explicitly link OpenSSL Crypto
)

# Add dependency for Python generation (if needed, depends on workflow)
# Ensure gRPC C++ generation happens before building the server
add_dependencies(matrix_server generate_python_grpc generate_grpc_cpp)

# --- Installation (Optional) ---
# install(TARGETS matrix_server DESTINATION bin)

# --- Output --- 
message(STATUS "Using C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Found Protobuf: ${Protobuf_FOUND}")
message(STATUS "Found gRPC: ${gRPC_FOUND}")
message(STATUS "Generated Proto Output Dir: ${PROTO_GEN_DIR}")
message(STATUS "Found Python3 Interpreter: ${Python3_EXECUTABLE}")
message(STATUS "Python Proto Output Dir: ${PYTHON_GEN_DIR}")
