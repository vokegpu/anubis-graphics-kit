#include "mesh_loader.hpp"
#include "api/util/env.hpp"
#include <fstream>

bool mesh_loader::load_object(mesh::data &data, std::string_view path) {
    std::ifstream ifs {path.data()};
    if (!ifs.is_open()) {
        util::log("failed to open model file '" + std::string(path.data()) + "'");
        return false;
    }

    std::string string_buffer {};
    std::string find {};
    std::string values {};

    size_t line_size {};
    uint8_t x_index {};
    uint8_t y_index {};
    uint8_t z_index {};

    switch (data.format) {
        case mesh::format::obj: {
            while (std::getline(ifs, string_buffer)) {
                line_size = string_buffer.size();

                if (line_size < 1) {
                    continue;
                }

                find = string_buffer.substr(0, 2);

                if (find == "v ") {
                    util::log(values);
                    values.replaceAll();

                    util::log(string_buffer.substr(x_index, y_index) + " " + string_buffer.substr(y_index, z_index) + " " + string_buffer.substr(z_index, line_size));


                    data.vert_amount++;
                } else if (find == "vt") {
                    x_index = string_buffer.find(' ') + 1;
                    y_index = string_buffer.find(' ', x_index + 1);
                    z_index = string_buffer.find(' ', y_index + 2);

                    data.texture_coordinates.push_back(static_cast<float>(std::stof(string_buffer.substr(x_index, y_index))));
                    data.texture_coordinates.push_back(static_cast<float>(std::stof(string_buffer.substr(y_index, z_index))));
                    data.texture_coordinates.push_back(static_cast<float>(std::stof(string_buffer.substr(z_index, line_size))));
                } else if (find == "vn") {
                    x_index = string_buffer.find(' ') + 1;
                    y_index = string_buffer.find(' ', x_index + 1);
                    z_index = string_buffer.find(' ', y_index + 2);

                    data.normals.push_back(static_cast<float>(std::stof(string_buffer.substr(x_index, y_index))));
                    data.normals.push_back(static_cast<float>(std::stof(string_buffer.substr(y_index, z_index))));
                    data.normals.push_back(static_cast<float>(std::stof(string_buffer.substr(z_index, line_size))));
                } else if (find == "f ") {
                    x_index = string_buffer.find(' ') + 1;
                    y_index = string_buffer.find(' ', x_index + 1);
                    z_index = string_buffer.find(' ', y_index + 2);

                    auto vert {string_buffer.substr(x_index, y_index - 1)};

                }
            }

            break;
        }

        case mesh::format::stl: {
            break;
        }

        default: {
            break;
        }
    }

    util::log("mesh loaded successfully!");
    ifs.close();
    return true;
}