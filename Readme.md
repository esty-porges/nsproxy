# First MVP
# 1. Run the server with:
#  * Load matrix from csv
#  * Create Zero matrix
#  * Create ID matrix
#  * List Matrices
#  * Matrix Transpose
#  * Check for equality of 2 Matrices
#  * Sum 2 matrices
#  * Mult 2 matrices into a third one
# 2. Random number generation on device
#   * Create Array
#   * Copy Array to device
#   * Random generation
#   * Copy Array Back to host
#   * Send Array to python.


# Some info on how to compile on dev server:

Remove the system module for openSSL before installing conan grpc
as the grpc is compiling again the system OpenSSL and this is causing an issue.

module load GCCcore/12.3.0 Clang/16.0.6 ccache/4.9.1 GDB/13.2 Perl/5.36.1
# Workaround for https://github.com/easybuilders/easybuild-framework/issues/3331
module del OpenSSL

. ./venv/bin/activate
 conan install . --output-folder=build --build=missing
 cmake . --preset conan-release
 
 cmake --build --preset conan-release
 ./build/build/Release/matrix_server 

 cd build/build/Release/
 cmake --build . 