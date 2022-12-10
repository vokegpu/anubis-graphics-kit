#include "mesh_loader.hpp"
#include "api/util/env.hpp"
#include <fstream>

void mesh_loader::process_indexing(mesh::data &data) {
    glm::vec3 v1 {}, v2 {}, v3 {}, n1 {}, n2;
    std::vector<uint32_t> nb_seen {};
    uint32_t v[3] {}, cur {};
    nb_seen.resize(this->vertex_positions_list.size(), 0);

    for (int32_t it {}; it < data.vertices_index.size(); it += 3) {
        uint32_t ia {data.vertices_index[it]}, ib {data.vertices_index[it + 1]}, ic {data.vertices_index[it + 2]};

        v1 = this->vertex_positions_list[ia];
        v2 = this->vertex_positions_list[ib];
        v3 = this->vertex_positions_list[ic];

        n1 = glm::normalize(glm::cross(v2 - v1, v3 - v1));
        v[0] = ia;
        v[1] = ib;
        v[2] = ic;

        for (int32_t it_cur {}; it_cur < 3; it_cur++) {
            cur = v[it_cur];
            nb_seen[cur]++;

            if (nb_seen[cur] == 1) {
                this->vertex_normals_list[cur] = n1;
            } else {
                n2 = this->vertex_normals_list[cur];
                n2.x = static_cast<float>(n2.x * (1.0 - 1.0 / nb_seen[cur]) + n1.x * 1.0 / nb_seen[cur]);
                n2.y = static_cast<float>(n2.y * (1.0 - 1.0 / nb_seen[cur]) + n1.y * 1.0 / nb_seen[cur]);
                n2.z = static_cast<float>(n2.z * (1.0 - 1.0 / nb_seen[cur]) + n1.z * 1.0 / nb_seen[cur]);
                this->vertex_normals_list[cur] = glm::normalize(n2);
            }
        }
    }

    for (glm::vec3 &normals : this->vertex_normals_list) {
        data.normals.push_back(normals.x);
        data.normals.push_back(normals.y);
        data.normals.push_back(normals.z);
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
    glm::vec3 vertex {};

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
                    vertex.x = std::stof(split[x]);
                    vertex.y = std::stof(split[y]);
                    vertex.z = std::stof(split[z]);

                    data.contains_vertices = true;
                    this->vertex_positions_list.push_back(vertex);

                    data.vertices.push_back(vertex.x);
                    data.vertices.push_back(vertex.y);
                    data.vertices.push_back(vertex.z);
                } else if (find == "vt") {
                    vertex.x = std::stof(split[x]);
                    vertex.y = std::stof(split[y]);

                    data.contains_texture_coordinates = true;
                    this->vertex_texture_coordinates_list.push_back(vertex);
                } else if (find == "vn") {
                    vertex.x = std::stof(split[x]);
                    vertex.y = std::stof(split[y]);
                    vertex.z = std::stof(split[z]);

                    data.contains_normals = true;
                    this->vertex_normals_list.push_back(vertex);
                } else if (find == "f ") {
                    /* get the faces vertex indexes  */

                    for (int32_t indexes {1}; indexes < split.size(); indexes++) {
                        util::split(split_first, split[indexes], '/');
                        data.faces_amount++;

                        /* send geometry indices to be used later in rendering */

                        if (data.contains_vertices) {
                            data.vertices_index.push_back(static_cast<uint32_t>(std::stoi(split_first[0])) - 1);
                        }

                        if (data.contains_texture_coordinates) {
                            data.texture_coordinates_index.push_back(static_cast<uint32_t>(std::stoi(split_first[1])) - 1);
                        }

                        if (data.contains_normals) {
                            // subtract using the boolean type @data.contains_texture_coordinates
                            data.normals_index.push_back(static_cast<uint32_t>(std::stoi(split_first[2 - (!data.contains_texture_coordinates)])) - 1);
                        }
                    }
                }
            }

            this->process_indexing(data);
            data.indexes = data.vertices_index;

            util::log("mesh loader collected indexes: " + std::to_string(data.indexes.size()));
            break;
        }

        case mesh::format::stl: {
            break;
        }

        default: {
            break;
        }
    }

    this->vertex_positions_list.clear();
    this->vertex_texture_coordinates_list.clear();
    this->vertex_normals_list.clear();

    util::log("mesh loaded successfully!");
    ifs.close();
    return true;
}