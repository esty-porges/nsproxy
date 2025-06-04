#include "../include/matrix_obj.h"

namespace matrix_service {

MatrixObj::MatrixObj(const std::string& n, int r, int c) 
    : name(n)
    , rows(r)
    , cols(c)
    , data(std::make_unique<float[]>(r * c)) {
    // Initialize with zeros
    std::fill(data.get(), data.get() + (r * c), 0.0f);
}

} // namespace matrix_service
