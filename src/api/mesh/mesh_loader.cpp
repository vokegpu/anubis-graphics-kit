#include "mesh_loader.hpp"
#include "api/util/env.hpp"
#include <fstream>

void mesh_loader::process_indexing_sequence(std::vector<int32_t> &indexes, std::vector<float> &data) {
    const std::vector<float> reference {data};

    for (int32_t &index : indexes) {
        data.push_back(reference[index - 1]);
    }
}

bool mesh_loader::load_object(mesh::data &data, std::string_view path) {
    std::ifstream ifs {path.data()};
    if (!ifs.is_open()) {
        util::log("failed to open model file '" + std::string(path.data()) + "'");
        return false;
    }

    std::string string_buffer {};
    std::string find {};
    std::string values {};
    std::vector<std::string> split {}, split_f_v {}, split_f_vt {}, split_f_vn {};

    size_t line_size {};
    size_t x {1};
    size_t y {2};
    size_t z {3};

    switch (data.format) {
        case mesh::format::obj: {
            while (std::getline(ifs, string_buffer)) {
                line_size = string_buffer.size();

                if (line_size < 1) {
                    continue;
                }

                find = string_buffer.substr(0, 2);
                util::split(split, string_buffer, ' ');

                if (split.size() != 4 && split.size() != 3) {
                    continue;
                }

                if (find == "v ") {
                    data.vertices.push_back(std::stof(split[x]));
                    data.vertices.push_back(std::stof(split[y]));
                    data.vertices.push_back(std::stof(split[z]));
                } else if (find == "vt") {
                    data.texture_coordinates.push_back(std::stof(split[x]));
                    data.texture_coordinates.push_back(std::stof(split[y]));
                } else if (find == "vn") {
                    data.normals.push_back(std::stof(split[x]));
                    data.normals.push_back(std::stof(split[y]));
                    data.normals.push_back(std::stof(split[z]));
                } else if (find == "f ") {
                    util::split(split_f_v, split[x], '/');
                    util::split(split_f_vt, split[y], '/');
                    util::split(split_f_vn, split[z], '/');

                    if (split_f_v.size() == 3 && split_f_vt.size() == 3 && split_f_vn.size() == 3) {
                        data.vertices_index.push_back(static_cast<uint32_t>(std::stoi(split_f_v[0])));
                        data.vertices_index.push_back(static_cast<uint32_t>(std::stoi(split_f_v[1])));
                        data.vertices_index.push_back(static_cast<uint32_t>(std::stoi(split_f_v[2])));
                        data.vert_amount += 3;

                        data.texture_coordinates_index.push_back(static_cast<uint32_t>(std::stoi(split_f_vt[0])));
                        data.texture_coordinates_index.push_back(static_cast<uint32_t>(std::stoi(split_f_vt[1])));
                        data.texture_coordinates_index.push_back(static_cast<uint32_t>(std::stoi(split_f_vt[2])));

                        data.normals_index.push_back(static_cast<uint32_t>(std::stoi(split_f_vn[0])));
                        data.normals_index.push_back(static_cast<uint32_t>(std::stoi(split_f_vn[1])));
                        data.normals_index.push_back(static_cast<uint32_t>(std::stoi(split_f_vn[2])));
                    }
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