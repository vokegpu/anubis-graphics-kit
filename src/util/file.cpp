#include "file.hpp"
#include "env.hpp"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

bool util::readfile(std::string_view path, std::string &string_builder) {
    std::ifstream ifs {path.data()};
    if (ifs.is_open()) {
        std::string string_buffer {};
        while (std::getline(ifs, string_buffer)) {
            string_builder += string_buffer;
            string_builder += '\n';
        }

        ifs.close();
        return true;
    } else {
        std::string log {"failed to open file '"};
        log += path;
        log += "'";
        util::log(log);
    }

    return false;
}

bool util::readimage(std::string_view path, int32_t &w, int32_t &h, void *&p_data) {
    int32_t channel {};
    return (p_data = stbi_load(path.data(), &w, &h, &channel, STBI_rgb));
}