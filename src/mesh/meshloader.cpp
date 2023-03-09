#include "meshloader.hpp"
#include "util/env.hpp"
#include <fstream>

mesh::serializer meshloader::convert_wavefront_object_mtllib_to_pbr(mesh::serializer &wavefront_object_mtllib_serializer) {
    auto &mtllib_metadata {wavefront_object_mtllib_serializer.get_metadata()};
    mesh::serializer pbr_mtl_serializer {};
    std::string mtl_name {};

    for (auto it_mtl {mtllib_metadata.begin()}; it_mtl != mtllib_metadata.end(); it_mtl = std::next(it_mtl)) {
        mtl_name = it_mtl->first;        
        for (auto it_content {it_mtl->second.begin()}; it_content != it_mtl->second.end(); it_content = std::next(it_content)) {
            if (it_content->first == "map_Ka" || it_content->first == "map_Kd" || it_content->first == "map_Ks") {
                pbr_mtl_serializer[mtl_name][this->mtl_pbr_conversions_map[it_content->first]] = it_content->second;
            }
        }
    }

    return pbr_mtl_serializer;
}

bool meshloader::load_model(mesh::data &data, std::string_view path) {
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
        case mesh::format::wavefrontobj: {
            std::ifstream ifs {};
            ifs.open(path.data());

            if (!ifs.is_open()) {
                std::string msg {"Failed to open/read OBJ Wavefront file: '"};
                msg += path;
                msg += "'";
                util::log(msg);
                return true;
            }

            this->load_wavefront_object(data, ifs);
            data.material_serializer = this->convert_wavefront_object_mtllib_to_pbr(this->material_serializer);
            ifs.close();

            return false;
        }

        case mesh::format::stl: {
            std::ifstream ifs {};
            ifs.open(path.data(), std::ifstream::in | std::ifstream::binary);

            if (!ifs.is_open()) {
                std::string msg {"Failed to open/read STL file: '"};
                msg += path;
                msg += "'";
                util::log(msg);
                return true;
            }
        
            this->load_stl_object(data, ifs);
            ifs.close();

            return false;
        }

        case mesh::format::gltf: {
            std::ifstream ifs(path.data());
            if (!ifs.is_open()) {
                std::string msg {"Failed to open/read GLTF file: '"};
                msg += path;
                msg += "'";
                util::log(msg);
                return true;
            }

            return false;
        }

        default: {
            util::log("Failed to load object mesh, unknown model format '" + file_extension + "'");
            break;
        }
    }

    return true;
}

void meshloader::load_wavefront_object(mesh::data &data, std::ifstream &ifs) {
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
    std::string mtlib_path {};

    while (std::getline(ifs, string_buffer)) {
        line_size = string_buffer.size();

        if (line_size < 1) {
            continue;
        }

        find = string_buffer.substr(0, std::min(6, (int32_t) line_size));
        if (find == "mtllib") {
            util::split(split_first, this->current_path, '/');

            uint64_t size {split_first[std::max((int32_t) split_first.size() - 1, 0)].size()};
            uint64_t tab_find {find.size()};

            std::string folder {this->current_path.substr(0, this->current_path.size() - size)};
            std::string file {string_buffer.substr(tab_find, string_buffer.size() - tab_find)};

            mtlib_path += folder;
            mtlib_path += file;
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
            this->material_serializer.mtllib_newmtl_list.push_back(split[x]);
        } else if (find == "v ") {
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
                data.faces++;

                if (v_contains) {
                    vsize = (uint32_t) pack.v.size() + 1;
                    index = (std::stoi(split_first[0]));
                    index = (index + vsize) % vsize;

                    data.append(mesh::type::vertex, pack.v[index - 1]);
                }

                if (t_contains) {
                    vsize = (uint32_t) pack.t.size() + 1;
                    index = (std::stoi(split_first[1]));
                    index = (index + vsize) % vsize;

                    data.append(mesh::type::textcoord, pack.t[index - 1]);
                }

                if (n_contains) {
                    vsize = (uint32_t) pack.n.size() + 1;
                    index = (std::stoi(split_first[2 - (!t_contains)]));
                    index = (index + vsize) % vsize;

                    data.append(mesh::type::normal, pack.n[index - 1]);
                }
            }
        }
    }

    if (!mtlib_path.empty()) {
        std::ifstream ifs_mtllib {mtlib_path};

        if (ifs_mtllib.is_open()) {
            this->load_wavefront_object_mtllib(data, ifs_mtllib);
        } else {
            util::log("Failed to open mtl lib material '" + mtlib_path + "'");
        }

        ifs_mtllib.close();
    }

    util::log("Mesh loader collector: Wavefront object faces (" + std::to_string(data.faces) + ")");
}

void meshloader::load_wavefront_object_mtllib(mesh::data &data, std::ifstream &ifs) {
    std::string string_buffer {};
    std::vector<std::string> split {};
    std::string newmtl_segment {};
    std::string metadata {};

    constexpr uint32_t x {1};
    constexpr uint32_t y {2};
    constexpr uint32_t z {3};

    while (std::getline(ifs, string_buffer)) {
        if (string_buffer.size() < 2) {
            continue;
        }

        if (string_buffer[0] == '\t') {
            string_buffer = string_buffer.substr(1, string_buffer.size() - 1);
            util::log(string_buffer);
        }

        util::split(split, string_buffer, ' ');

        if (split[0] == "illum" || split[0] == "Ns" || split[0] == "Ni" || split[0] == "d" || split[0] == "Tr") {
            this->material_serializer[newmtl_segment][split[0]] = split[x];
        } else if (split[0] == "newmtl") {
            metadata.clear();
            metadata += string_buffer.substr(split[0].size() + 1, string_buffer.size() - split[0].size() - 1);
            this->material_serializer[metadata];
        } else if (split[0] == "Ka" || split[0] == "Kd" || split[0] == "Ks" || split[0] == "Ke" || split[0] == "Tf") {
            metadata.clear();
            metadata += split[x];
            metadata += '/';
            metadata += split[y];
            metadata += '/';
            metadata += split[z];
            this->material_serializer[newmtl_segment][split[0]] = metadata;
        } else if (split[0] == "bump" || split[0] == "map_bump" || split[0] == "map_Ka" || split[0] == "map_Kd" || split[0] == "map_Ks" || split[0] == "map_Ke") {
            metadata.clear();
            metadata += string_buffer.substr(split[0].size() + 1, string_buffer.size() - split[0].size() - 1);
            this->material_serializer[newmtl_segment][split[0]] = metadata;
        }
    }
}

void meshloader::load_stl_object(mesh::data &data, std::ifstream &ifs) {
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

bool meshloader::load_heightmap(mesh::data &data, util::image &resource) {
    unsigned char r {}, g {}, b {};
    float f_r {}, f_g {}, f_b {};
    float greyscale{};

    const uint32_t width {static_cast<uint32_t>(resource.w)};
    const uint32_t height {static_cast<uint32_t>(resource.h)};
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

bool meshloader::load_identity_heightmap(mesh::data &data, uint32_t width, uint32_t height) {
    glm::vec3 vertex {};
    data.faces = static_cast<int32_t>(4 * width * height);

    const float fwf {static_cast<float>(width)};
    const float fhf {static_cast<float>(height)};

    for (float h {}; h <= static_cast<float>(height - 1); h++) {
        for (float w {}; w <= static_cast<float>(width - 1); w++) {
            /* first vertex of quad */
            vertex.x = w / fwf;
            vertex.y = 0.0f;
            vertex.z = h / fhf;

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, vertex.x);
            data.append(mesh::type::textcoord, vertex.z);

            /* second vertex of quad */
            vertex.x = (w + 1.0f) / fwf;
            vertex.z = h / hfh;

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, vertex.x);
            data.append(mesh::type::textcoord, vertex.z);

            /* third vertex of quad */
            vertex.x = w / fwf;
            vertex.y = 0.0f;
            vertex.z = (h + 1.0f) / hfh;

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, vertex.x);
            data.append(mesh::type::textcoord, vertex.z);

            /* four vertex of quad */

            /* third vertex of quad */
            vertex.x = (w 1.0f) / fwf;
            vertex.y = 0.0f;
            vertex.z = (h + 1.0f) / hfh;

            data.append(mesh::type::vertex, vertex);
            data.append(mesh::type::textcoord, vertex.x);
            data.append(mesh::type::textcoord, vertex.z);
        }
    }

    return false;
}