#ifndef AGK_API_UTIL_FILE_H
#define AGK_API_UTIL_FILE_H

#include <iostream>

namespace util {
    struct image {
        std::string path {};
        int32_t w {};
        int32_t h {};
        int32_t format {};
        uint8_t *p_data {};
    };

    bool read_file(std::string_view path, std::string &string_builder);
    bool read_image(std::string_view path, util::image &resource);
}

#endif