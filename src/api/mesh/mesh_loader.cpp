#include "mesh_loader.hpp"
#include "api/util/env.hpp"
#include <fstream>

void mesh_loader::process_indexing_sequence(std::vector<uint32_t> &indexes, std::vector<float> &data) {
    const std::vector<float> reference {data};
    data.clear();

    for (uint32_t &index : indexes) {
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
    std::vector<std::string> split {}, split_first {}, split_second {}, split_third {};

    size_t line_size {};
    size_t x {1};
    size_t y {2};
    size_t z {3};

    bool contains_normals {};

    switch (data.format) {
        case mesh::format::obj: {
            while (std::getline(ifs, string_buffer)) {
                line_size = string_buffer.size();

                if (line_size < 1) {
                    continue;
                }

                find = string_buffer.substr(0, 2);

                if (find != "v " && find != "vt" && find != "vn" && find != "f ") {
                    continue;
                }

                util::split(split_first, string_buffer, ' ');

                if (split_first.size() < 2) {
                    continue;
                }

                split.clear();

                for (std::string &s : split_first) {
                    if (!s.empty()) {
                        split.push_back(s);
                    }
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
                    contains_normals = true;
                } else if (find == "f ") {
                    util::split(split_first, split[x], '/');
                    util::split(split_second, split[y], '/');
                    util::split(split_third, split[z], '/');

                    if (split_first.size() >= 2 && split_second.size() >= 2 && split_third.size() >= 2) {
                        // the reason for subtract -1: opengl starts with 0 not 1.
                        data.vertices_index.push_back(static_cast<uint32_t>(std::stoi(split_first[0])) - 1);
                        data.vertices_index.push_back(static_cast<uint32_t>(std::stoi(split_second[0])) - 1);
                        data.vertices_index.push_back(static_cast<uint32_t>(std::stoi(split_third[0])) - 1);
                        data.vert_amount += 3;

                        data.texture_coordinates_index.push_back(static_cast<uint32_t>(std::stoi(split_first[1])) - 1);
                        data.texture_coordinates_index.push_back(static_cast<uint32_t>(std::stoi(split_second[1])) - 1);
                        data.texture_coordinates_index.push_back(static_cast<uint32_t>(std::stoi(split_third[1])) - 1);

                        if (contains_normals) {
                            data.normals_index.push_back(static_cast<uint32_t>(std::stoi(split_first[2])) - 1);
                            data.normals_index.push_back(static_cast<uint32_t>(std::stoi(split_second[2])) - 1);
                            data.normals_index.push_back(static_cast<uint32_t>(std::stoi(split_third[2])) - 1);
                        }
                    }
                }
            }

            util::log("mesh loader collected indexes: " + std::to_string(data.vert_amount));
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