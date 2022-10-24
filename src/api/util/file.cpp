#include "file.hpp"
#include "env.hpp"
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
    } else {
        std::string log {"failed to open file '"};
        log += path;
        log += "'";
        util::log(log);
    }

    return false;
}
