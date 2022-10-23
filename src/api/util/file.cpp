#include "file.hpp"
#include <fstream>

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
    }

    return false;
}
