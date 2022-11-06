#include "mesh_loader.hpp"
#include "api/util/env.hpp"
#include <fstream>

void mesh_loader::process_indexing(mesh::data &data) {
    const std::vector<mesh::vertex> vertex_pos_copy {this->vertex_positions_list}, vertex_tex_copy {this->vertex_texture_coordinates_list}, vertex_norm_copy {this->vertex_normals_list};

    this->vertex_positions_list.clear();
    this->vertex_texture_coordinates_list.clear();
    this->vertex_normals_list.clear();

    for (uint32_t index {}; index < data.faces_amount; index++) {
        if (data.contains_vertices) {
            this->vertex_positions_list.emplace_back() = vertex_pos_copy[data.vertices_index[index]];
        }

        if (data.contains_texture_coordinates) {
            this->vertex_texture_coordinates_list.emplace_back() = vertex_tex_copy[data.texture_coordinates_index[index]];
        }

        if (data.contains_normals) {
            this->vertex_normals_list.emplace_back() = vertex_norm_copy[data.normals_index[index]];
        }
    }

    std::map<mesh::packed_vertex, uint32_t> packed_vertexes_map {};

    data.vertices.clear();
    data.texture_coordinates.clear();
    data.normals.clear();

    for (uint32_t index {}; index < data.faces_amount; index++) {
        mesh::packed_vertex packed_vertex {};

        if (data.contains_vertices) {
            packed_vertex.vert = this->vertex_positions_list[index];
        }

        if (data.contains_texture_coordinates) {
            packed_vertex.text = this->vertex_texture_coordinates_list[index];
        }

        if (data.contains_normals) {
            packed_vertex.norm = this->vertex_normals_list[index];
        }

        auto it {packed_vertexes_map.find(packed_vertex)};
        bool found {it == packed_vertexes_map.end()};

        if (!found) {
            data.indexes.push_back(it->second);
            continue;
        }

        if (data.contains_vertices) {
            data.vertices.push_back(packed_vertex.vert.x);
            data.vertices.push_back(packed_vertex.vert.y);
            data.vertices.push_back(packed_vertex.vert.z);
        }

        if (data.contains_texture_coordinates) {
            data.texture_coordinates.push_back(packed_vertex.text.x);
            data.texture_coordinates.push_back(packed_vertex.text.y);
        }
        
        if (data.contains_normals) {
            data.normals.push_back(packed_vertex.norm.x);
            data.normals.push_back(packed_vertex.norm.y);
            data.normals.push_back(packed_vertex.norm.z);
        }

        data.indexes.push_back((packed_vertexes_map[packed_vertex] = data.indexes.size() - 1));
    }

    bool s {};
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
    mesh::vertex vertex {};

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
                    vertex.length = 3;

                    this->vertex_positions_list.push_back(vertex);
                    data.contains_vertices = true;
                } else if (find == "vt") {
                    vertex.x = std::stof(split[x]);
                    vertex.y = std::stof(split[y]);
                    vertex.length = 2;

                    this->vertex_texture_coordinates_list.push_back(vertex);
                    data.contains_texture_coordinates = true;
                } else if (find == "vn") {
                    vertex.x = std::stof(split[x]);
                    vertex.y = std::stof(split[y]);
                    vertex.z = std::stof(split[z]);
                    vertex.length = 3;

                    this->vertex_normals_list.push_back(vertex);
                    data.contains_normals = true;
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