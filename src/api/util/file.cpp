#include "file.hpp"
#include "env.hpp"
#include <fstream>
#include <stb/stb_image.h>

bool util::read_file(std::string_view path, std::string &string_builder) {
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

bool util::read_image(std::string_view path, util::image &resource) {
    resource.p_data = stbi_load(path.data(), &resource.w, &resource.h, &resource.format, 0);
    resource.path = path;

    if (!resource.p_data) {
        std::string message {"Failed to read image file '"};
        message += path;
        message += "'";
        util::log(message);
        return true;
    }

    return false;
}

void util::free_image(util::image &resource) {
    stbi_image_free(resource.p_data);
}
