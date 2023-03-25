#ifndef AGK_UTIL_FILE_H
#define AGK_UTIL_FILE_H

#include <iostream>
#include "gpu/gpu.hpp"

namespace util {
    bool readfile(std::string_view path, std::string &string_builder);
    bool readimage(std::string_view path, int32_t &w, int32_t &h, void *&p_data);
}

#endif