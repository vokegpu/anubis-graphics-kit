#include "mesh_loader.hpp"
#include "util/env.hpp"
#include <fstream>

bool mesh_loader::load_object(mesh::data &data, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        util::log("Failed to load object because there is no path insert");
        return true;
    }

    std::string file_extension {};
    this->current_path = path;

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

    meshpack pack {};
    uint32_t index {}, vsize {};

    while (std::getline(ifs, string_buffer)) {
        line_size = string_buffer.size();

        if (line_size < 1) {
            continue;
        }

        find = string_buffer.substr(0, std::min(6, (int32_t) line_size));
        if (find == "mtllib") {
            util::split(split_first, this->current_path, '/');

            uint64_t size {split_first[std::max((int32_t) split_first.size() - 1, 0)].size()};
            uint64_t tab_find {6 + 1};

            std::string folder {this->current_path.substr(0, this->current_path.size() - size)};
            std::string file {string_buffer.substr(tab_find, string_buffer.size() - tab_find)};
            std::ifstream ifs_mtllib {folder + file};

            if (ifs_mtllib.is_open()) {
                this->load_wavefront_object_mtllib(pack.mesh, ifs_mtllib);
            } else {
                util::log("Failed to open mtl lib material '" + (folder + file) + "'");
            }

            ifs_mtllib.close();
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

            pack.v.push_back(vector3f);
            v_contains = true;
        } else if (find == "vt") {
            vector2f.x = std::stof(split[x]);
            vector2f.y = std::stof(split[y]);

            pack.t.push_back(vector2f);
            t_contains = true;
        } else if (find == "vn") {
            vector3f.x = std::stof(split[x]);
            vector3f.y = std::stof(split[y]);
            vector3f.z = std::stof(split[z]);

            pack.n.push_back(vector3f);
            n_contains = true;
        } else if (find == "f ") {
            for (int32_t indexes {1}; indexes < split.size(); indexes++) {
                util::split(split_first, split[indexes], '/');
                pack.mesh.faces++;

                if (v_contains) {
                    vsize = (uint32_t) pack.v.size() + 1;
                    index = (std::stoi(split_first[0]));
                    index = (index + vsize) % vsize;

                    pack.mesh.append(mesh::type::vertex, pack.v[index - 1]);
                }

                if (t_contains) {
                    vsize = (uint32_t) pack.t.size() + 1;
                    index = (std::stoi(split_first[1]));
                    index = (index + vsize) % vsize;

                    pack.mesh.append(mesh::type::textcoord, pack.t[index - 1]);
                }

                if (n_contains) {
                    vsize = (uint32_t) pack.n.size() + 1;
                    index = (std::stoi(split_first[2 - (!t_contains)]));
                    index = (index + vsize) % vsize;

                    pack.mesh.append(mesh::type::normal, pack.n[index - 1]);
                }
            }
        }
    }

    data = pack.mesh;
    util::log("Mesh loader collector: Wavefront object faces (" + std::to_string(data.faces) + ")");
}

void mesh_loader::load_wavefront_object_mtllib(mesh::data &data, std::ifstream &ifs) {
    std::string string_buffer {};
    std::string mtl_string_info {};
    std::vector<std::string> split {};

    while (std::getline(ifs, string_buffer)) {
        if (string_buffer.size() < 2) {
            continue;
        }

        util::split(split, string_buffer, ' ');
        if (split[0] == "map_Kd") {
            mtl_string_info = string_buffer.substr(split[0].size() + 1, string_buffer.size() - split[0].size() - 1);
            data.mtl_texture_list.push_back(mtl_string_info);
            util::log("Found mtl map_Kd '" + mtl_string_info + "'");
        }
    }
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
    unsigned char r{}, g{}, b{};
    float f_r{}, f_g{}, f_b{};
    float greyscale{};

    uint32_t width{static_cast<uint32_t>(resource.w)};
    uint32_t height{static_cast<uint32_t>(resource.h)};
    glm::vec3 vertex{};

    for (uint32_t h{}; h < height; h++) {
        for (uint32_t w{}; w < width; w++) {
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

    for (uint32_t h{}; h < height - 1; h++) {
        for (uint32_t w{}; w < width - 1; w++) {
            uint32_t i1{h * width + w};
            uint32_t i2{h * width + w + 1};
            uint32_t i3{(h + 1) * width + w};
            uint32_t i4{(h + 1) * width + w + 1};

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
            vertex.x = static_cast<float>(w) / static_cast<float>(width);
            vertex.y = 0.0f;
            vertex.z = static_cast<float>(h) / static_cast<float>(height);

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, static_cast<float>(w) / static_cast<float>(width));
            data.append(mesh::type::textcoord, static_cast<float>(h) / static_cast<float>(height));

            /* second vertex of quad */
            vertex.x = static_cast<float>(w + 1) / static_cast<float>(width);
            vertex.y = 0.0f;
            vertex.z = static_cast<float>(h) / static_cast<float>(height);

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, static_cast<float>(w + 1) / static_cast<float>(width));
            data.append(mesh::type::textcoord, static_cast<float>(h) / static_cast<float>(height));

            /* third vertex of quad */
            vertex.x = static_cast<float>(w) / static_cast<float>(width);
            vertex.y = 0.0f;
            vertex.z = static_cast<float>(h + 1) / static_cast<float>(height);

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, static_cast<float>(w) / static_cast<float>(width));
            data.append(mesh::type::textcoord, static_cast<float>(h + 1) / static_cast<float>(height));

            /* four vertex of quad */
            vertex.x = static_cast<float>(w + 1) / static_cast<float>(width);
            vertex.y = 0.0f;
            vertex.z = static_cast<float>(h + 1) / static_cast<float>(height);

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, static_cast<float>(w + 1) / static_cast<float>(width));
            data.append(mesh::type::textcoord, static_cast<float>(h + 1) / static_cast<float>(height));
        }
    }

    return false;
}