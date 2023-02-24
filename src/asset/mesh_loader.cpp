#include "mesh_loader.hpp"
#include "util/env.hpp"
#include <fstream>

void mesh_loader::process_indexing(meshpack &pack, uint32_t length) {
    glm::vec3 v1 {}, v2 {}, v3 {}, n1 {}, n2;
    std::vector<uint32_t> nb_seen {};
    uint32_t v[3] {}, cur {};
    nb_seen.resize(pack.v.size(), 0);

    auto &i_list {pack.mesh.get_uint_list(mesh::type::vertex)};
    bool contains_n {pack.mesh.contains(mesh::type::normal)};
    pack.mesh.get_float_list(mesh::type::normal).clear();

    pack.n.resize(i_list.size());
    uint32_t ia {}, ib {}, ic {};

    for (int32_t it {}; it < i_list.size(); it += 3) {
        ia = i_list[it + 0];
        ib = i_list[it + 1];
        ic = i_list[it + 2];

        v1 = pack.n[ia];
        v2 = pack.n[ib];
        v3 = pack.n[ic];

        n1 = glm::normalize(glm::cross(v2 - v1, v3 - v1));
        v[0] = ia;
        v[1] = ib;
        v[2] = ic;

        if (!contains_n) {
            pack.n.push_back(n1);
        }

        for (int32_t it_cur {}; it_cur < 3; it_cur++) {
            cur = v[it_cur];
            nb_seen[cur]++;

            if (nb_seen[cur] == 1) {
                pack.n[cur] = n1;
            } else {
                n2 = pack.n[cur];
                n2.x = static_cast<float>(n2.x * (1.0 - 1.0 / nb_seen[cur]) + n1.x * 1.0 / nb_seen[cur]);
                n2.y = static_cast<float>(n2.y * (1.0 - 1.0 / nb_seen[cur]) + n1.y * 1.0 / nb_seen[cur]);
                n2.z = static_cast<float>(n2.z * (1.0 - 1.0 / nb_seen[cur]) + n1.z * 1.0 / nb_seen[cur]);
                pack.n[cur] = glm::normalize(n2);
            }
        }
    }

    for (glm::vec3 &n : pack.n) {
        pack.mesh.append(mesh::type::normal, n);
    }
}

bool mesh_loader::load_object(mesh::data &data, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        util::log("Failed to load object because there is no path insert");
        return true;
    }

    std::string file_extension {};
    if (data.format == ::mesh::format::unknown) {
        std::vector<std::string> split_path {};
        util::split(split_path, path, '.');
        split_path.emplace_back();
        file_extension = split_path[std::max((int32_t) split_path.size() - 2, 0)];
        data.format = !this->mesh_ext_map.count(file_extension) ? data.format : this->mesh_ext_map[file_extension];
    }

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
            util::log("Failed to load object mesh, unknown model format '" + file_extension + "'");
            break;
        }
    }

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

    //std::vector<meshpack> mesh_pack_list {};
    //mesh_pack_list.emplace_back();
    uint32_t current_pack {};
    bool first_pack {true};
    meshpack pack {};

    while (std::getline(ifs, string_buffer)) {
        line_size = string_buffer.size();

        if (line_size < 1) {
            continue;
        }

        find = string_buffer.substr(0, 2);

        if (find != "v " && find != "vt" && find != "vn" && find != "f " && find != "o ") {
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

        if (find == "o ") {
        } else if (find == "v ") {
            vector3f.x = std::stof(split[x]);
            vector3f.y = std::stof(split[y]);
            vector3f.z = std::stof(split[z]);

            pack.v.push_back(vector3f);
            pack.mesh.append(mesh::type::vertex, vector3f);
        } else if (find == "vt") {
            vector2f.x = std::stof(split[x]);
            vector2f.y = std::stof(split[y]);

            pack.t.push_back(vector2f);
            pack.mesh.append(mesh::type::textcoord, vector2f);
        } else if (find == "vn") {
            vector3f.x = std::stof(split[x]);
            vector3f.y = std::stof(split[y]);
            vector3f.z = std::stof(split[z]);

            pack.n.push_back(vector3f);
            pack.mesh.append(mesh::type::normal, vector3f);
        } else if (find == "f ") {
            /* get the faces vertex indexes  */
            v_contains = pack.mesh.contains(mesh::type::vertex);
            t_contains = pack.mesh.contains(mesh::type::textcoord);
            n_contains = pack.mesh.contains(mesh::type::normal);

            for (int32_t indexes {1}; indexes < split.size(); indexes++) {
                util::split(split_first, split[indexes], '/');
                pack.mesh.faces++;

                /* send geometry indices to be used later in rendering */

                if (v_contains) {
                    pack.mesh.append(mesh::type::vertex, static_cast<uint32_t>(abs(std::stoi(split_first[0]))) - 1);
                }

                if (t_contains) {
                    pack.mesh.append(mesh::type::textcoord, static_cast<uint32_t>(abs(std::stoi(split_first[1]))) - 1);
                }

                if (n_contains) {
                    pack.mesh.append(mesh::type::normal, static_cast<uint32_t>(abs(std::stoi(split_first[2 - (!t_contains)]))) - 1);
                }
            }
        }
    }

    this->process_indexing(pack, 0);
    data = pack.mesh;

    util::log("Mesh loader collector: Wavefront object faces (" + std::to_string(data.faces) + ")");
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

bool mesh_loader::load_heightmap(mesh::data &data, util::image &resource) {
    unsigned char r {}, g {}, b {};
    float f_r {}, f_g {}, f_b {};
    float greyscale {};

    uint32_t width {static_cast<uint32_t>(resource.w)};
    uint32_t height {static_cast<uint32_t>(resource.h)};
    glm::vec3 vertex {};

    for (uint32_t h {}; h < height; h++) {
        for (uint32_t w {}; w < width; w++) {
            r = resource.p_data[(w + h * width) * resource.format];
            g = resource.p_data[(w + h * width) * resource.format + 1];
            b = resource.p_data[(w + h * width) * resource.format + 2];

            f_r = static_cast<float>(r) / 255;
            f_g = static_cast<float>(g) / 255;
            f_b = static_cast<float>(b) / 255;

            greyscale = (f_r + f_g + f_b) / 3;
            vertex.x = (static_cast<float>(w));
            vertex.y = r;
            vertex.z = (static_cast<float>(h));

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::color, {greyscale, greyscale, greyscale});
        }
    }

    for (uint32_t h {}; h < height - 1; h++) {
        for (uint32_t w {}; w < width - 1; w++) {
            uint32_t i1 {h * width + w};
            uint32_t i2 {h * width + w + 1};
            uint32_t i3 {(h + 1) * width + w};
            uint32_t i4 {(h + 1) * width + w + 1};

            data.append(mesh::type::vertex, i1);
            data.append(mesh::type::vertex, i3);
            data.append(mesh::type::vertex, i2);

            data.append(mesh::type::vertex, i2);
            data.append(mesh::type::vertex, i3);
            data.append(mesh::type::vertex, i4);
            data.faces += 6;
        }
    }

    util::log("Processed base heightmap with " + std::to_string(data.faces) + " faces");
    return false;
}

bool mesh_loader::load_identity_heightmap(mesh::data &data, uint32_t width, uint32_t height) {
    glm::vec3 vertex {};
    data.faces = static_cast<int32_t>(4 * width * height);

    for (uint32_t h {}; h <= height - 1; h++) {
        for (uint32_t w {}; w <= width - 1; w++) {
            /* first vertex of quad */
            vertex.x = static_cast<float>(w);
            vertex.y = 0.0f;
            vertex.z = static_cast<float>(h);

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, static_cast<float>(w) / static_cast<float>(width));
            data.append(mesh::type::textcoord, static_cast<float>(h) / static_cast<float>(height));

            /* second vertex of quad */
            vertex.x = static_cast<float>(w + 1);
            vertex.y = 0.0f;
            vertex.z = static_cast<float>(h);

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, static_cast<float>(w + 1) / static_cast<float>(width));
            data.append(mesh::type::textcoord, static_cast<float>(h) / static_cast<float>(height));

            /* third vertex of quad */
            vertex.x = static_cast<float>(w);
            vertex.y = 0.0f;
            vertex.z = static_cast<float>(h + 1);

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, static_cast<float>(w) / static_cast<float>(width));
            data.append(mesh::type::textcoord, static_cast<float>(h + 1) / static_cast<float>(height));

            /* four vertex of quad */
            vertex.x = static_cast<float>(w + 1);
            vertex.y = 0.0f;
            vertex.z = static_cast<float>(h + 1);

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, static_cast<float>(w + 1) / static_cast<float>(width));
            data.append(mesh::type::textcoord, static_cast<float>(h + 1) / static_cast<float>(height));
        }
    }

    return false;
}
