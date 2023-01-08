#include "mesh_loader.hpp"
#include "api/util/env.hpp"
#include <fstream>

void mesh_loader::process_indexing(mesh::data &data) {
    glm::vec3 v1 {}, v2 {}, v3 {}, n1 {}, n2;
    std::vector<uint32_t> nb_seen {};
    uint32_t v[3] {}, cur {};
    nb_seen.resize(this->v_packed_list.size(), 0);

    auto &i_list {data.get_uint_list(mesh::type::vertex)};
    data.get_float_list(mesh::type::normal).clear();
    bool contains_n {data.contains(mesh::type::normal)};

    this->n_packed_list.resize(i_list.size());

    for (int32_t it {}; it < i_list.size(); it += 3) {
        uint32_t ia {i_list[it]}, ib {i_list[it + 1]}, ic {i_list[it + 2]};

        v1 = this->v_packed_list[ia];
        v2 = this->v_packed_list[ib];
        v3 = this->v_packed_list[ic];

        n1 = glm::normalize(glm::cross(v2 - v1, v3 - v1));
        v[0] = ia;
        v[1] = ib;
        v[2] = ic;

        if (!contains_n) {
            this->n_packed_list.push_back(n1);
        }

        for (int32_t it_cur {}; it_cur < 3; it_cur++) {
            cur = v[it_cur];
            nb_seen[cur]++;

            if (nb_seen[cur] == 1) {
                this->n_packed_list[cur] = n1;
            } else {
                n2 = this->n_packed_list[cur];
                n2.x = static_cast<float>(n2.x * (1.0 - 1.0 / nb_seen[cur]) + n1.x * 1.0 / nb_seen[cur]);
                n2.y = static_cast<float>(n2.y * (1.0 - 1.0 / nb_seen[cur]) + n1.y * 1.0 / nb_seen[cur]);
                n2.z = static_cast<float>(n2.z * (1.0 - 1.0 / nb_seen[cur]) + n1.z * 1.0 / nb_seen[cur]);
                this->n_packed_list[cur] = glm::normalize(n2);
            }
        }
    }

    for (glm::vec3 &n : this->n_packed_list) {
        data.append(mesh::type::normal, n);
    }
}

bool mesh_loader::load_object(mesh::data &data, std::string_view path) {
    switch (data.format) {
        case mesh::format::obj: {
            std::ifstream ifs {};
            ifs.open(path.data());

            if (!ifs.is_open()) {
                util::log("Failed to open/read object wavefront file: '" + std::string(path.data()) + "'");
                return true;
            }

            this->load_wavefront_object(data, ifs);
            ifs.close();

            return false;
        }

        case mesh::format::stl: {
            std::ifstream ifs {};
            ifs.open(path.data(), std::ifstream::in | std::ifstream::binary);

            if (!ifs.is_open()) {
                util::log("Failed to open/read stl file: '" + std::string(path.data()) + "'");
                return true;
            }
        
            this->load_stl_object(data, ifs);
            ifs.close();

            return false;
        }

        default: {
            break;
        }
    }

    this->v_packed_list.clear();
    this->t_packed_list.clear();
    this->n_packed_list.clear();
    return true;
}

void mesh_loader::load_wavefront_object(mesh::data &data, std::ifstream &ifs) {
    std::string string_buffer {};
    std::string find {};
    std::string values {};
    std::vector<std::string> split {}, split_first {}, split_second {}, split_third {};

    size_t line_size {};
    size_t x {1};
    size_t y {2};
    size_t z {3};

    bool v_contains {};
    bool t_contains {};
    bool n_contains {};

    glm::vec3 vector3f {};
    glm::vec2 vector2f {};

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
            vector3f.x = std::stof(split[x]);
            vector3f.y = std::stof(split[y]);
            vector3f.z = std::stof(split[z]);

            this->v_packed_list.push_back(vector3f);
            data.append(mesh::type::vertex, vector3f);
        } else if (find == "vt") {
            vector3f.x = std::stof(split[x]);
            vector3f.y = std::stof(split[y]);

            vector2f.x = vector3f.x;
            vector2f.y = vector3f.y;

            this->t_packed_list.push_back(vector3f);
            data.append(mesh::type::textcoord, vector2f);
        } else if (find == "vn") {
            vector3f.x = std::stof(split[x]);
            vector3f.y = std::stof(split[y]);
            vector3f.z = std::stof(split[z]);

            this->n_packed_list.push_back(vector3f);
            data.append(mesh::type::normal, vector3f);
        } else if (find == "f ") {
            /* get the faces vertex indexes  */
            v_contains = data.contains(mesh::type::vertex);
            t_contains = data.contains(mesh::type::textcoord);
            n_contains = data.contains(mesh::type::normal);

            for (int32_t indexes {1}; indexes < split.size(); indexes++) {
                util::split(split_first, split[indexes], '/');
                data.faces++;

                /* send geometry indices to be used later in rendering */

                if (v_contains) {
                    data.append(mesh::type::vertex, static_cast<uint32_t>(std::stoi(split_first[0])) - 1);
                }

                if (t_contains) {
                    data.append(mesh::type::textcoord, static_cast<uint32_t>(std::stoi(split_first[1])) - 1);
                }

                if (n_contains) {
                    data.append(mesh::type::normal, static_cast<uint32_t>(std::stoi(split_first[2 - (!t_contains)])) - 1);
                }
            }
        }
    }

    util::log("Mesh loader collector: Wavefront object faces (" + std::to_string(data.faces) + ")");
    this->process_indexing(data);
}

void mesh_loader::load_stl_object(mesh::data &data, std::ifstream &ifs) {
    ifs.seekg(80);
    ifs.read((char*) &data.faces, sizeof(int32_t));

    char unused_byte[2] {};
    float val {};

    glm::vec3 triangle_v1 {};
    glm::vec3 triangle_v2 {};
    glm::vec3 triangle_v3 {};
    glm::vec3 triangle_n1 {};

    for (int32_t it {}; it < data.faces; it++) {
        ifs.read((char*) &val, 4);
        triangle_n1.x = val;

        ifs.read((char*) &val, 4);
        triangle_n1.y = val;

        ifs.read((char*) &val, 4);
        triangle_n1.z= val;

        /* First vertex of triangle face. */
        ifs.read((char*) &val, 4);
        triangle_v1.x = val;

        ifs.read((char*) &val, 4);
        triangle_v1.y = val;

        ifs.read((char*) &val, 4);
        triangle_v1.z = val;

        /* Second vertex of triangle face. */
        ifs.read((char*) &val, 4);
        triangle_v2.x = val;

        ifs.read((char*) &val, 4);
        triangle_v2.y = val;

        ifs.read((char*) &val, 4);
        triangle_v2.z = val;

        /* Third vertex of triangle face. */
        ifs.read((char*) &val, 4);
        triangle_v3.x = val;

        ifs.read((char*) &val, 4);
        triangle_v3.y = val;

        ifs.read((char*) &val, 4);
        triangle_v3.z = val;

        /* End of face collect. */
        ifs.read(unused_byte, 2);

        data.append(mesh::type::vertex, triangle_v1);
        data.append(mesh::type::vertex, triangle_v2);
        data.append(mesh::type::vertex, triangle_v3);

        data.append(mesh::type::normal, triangle_n1);
        data.append(mesh::type::normal, triangle_n1);
        data.append(mesh::type::normal, triangle_n1);
    }
    
    util::log("Mesh loader collector: STL faces (" + std::to_string(data.faces) + ")");
}
