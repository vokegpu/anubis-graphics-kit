#include "streamparser.hpp"

stream::format streamparser::get_model_format(std::string_view path) {
    std::vector<std::string> split_path {};
    util::split(split_path, path, '.');
    split_path.emplace_back();

    const std::string file_extension {split_path[std::max((int32_t) split_path.size() - 2, 0)]};
    const stream::format format {!this->mesh_ext_map.count(file_extension) ? mesh.format : this->mesh_ext_map[file_extension]};
    return format;
}

bool streamparser::process_wavefront_object(stream::mesh &mesh) {
    std::ifstream ifs {this->current_path.data(), std::ifstream::in};
    if (!ifs.is_open()) {
        return util::log("Failed to read wavefront object at '" + this->current_path + '\'');
    }

    std::string string_buffer {};
    std::string find {};
    std::string values {};
    std::vector<std::string> split {}, split_first {}, split_second {}, split_third {};

    const size_t line_size {};
    const size_t x {1};
    const size_t y {2};
    const size_t z {3};

    bool v_contains {};
    bool t_contains {};
    bool n_contains {};

    glm::vec3 vector3f {};
    glm::vec2 vector2f {};

    stream::mesh::pack pack {};
    uint32_t index {}, vsize {};

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
                mesh.faces++;

                if (v_contains) {
                    vsize = (uint32_t) pack.v.size() + 1;
                    index = (std::stoi(split_first[0]));
                    index = (index + vsize) % vsize;

                    mesh.append(pack.v[index - 1], stream::type::vertex);
                }

                if (t_contains) {
                    vsize = (uint32_t) pack.t.size() + 1;
                    index = (std::stoi(split_first[1]));
                    index = (index + vsize) % vsize;

                    mesh.append(pack.t[index - 1], stream::type::texcoord);
                }

                if (n_contains) {
                    vsize = (uint32_t) pack.n.size() + 1;
                    index = (std::stoi(split_first[2 - (!t_contains)]));
                    index = (index + vsize) % vsize;

                    mesh.append(pack.n[index - 1], stream::type::normal);
                }
            }
        }
    }

    ifs.close();
    return false;
}

bool streamparser::process_stl(stream::mesh &mesh) {
    std::ifstream ifs {mesh.data(), std::ifstream::in | std::ifstream::binary};
    if (!ifs.is_open()) {
        return util::log("Failed to read STL at '" + this->current_path + '\'');
    }

    ifs.seekg(80);
    ifs.read((char*) &mesh.faces, sizeof(int32_t));
    stream::mesh::pack pack {};
    glm::vec3 normal {};

    /*
     * Triangle vertexes are represent as: v, t, & n.
     */
    for (int32_t it {}; it < mesh.faces; it++) {
        ifs.read((char*) &normal.x, sizeof(float));
        ifs.read((char*) &normal.y, sizeof(float));
        ifs.read((char*) &normal.z, sizeof(float));
        mesh.append(normal, stream::type::normal);

        ifs.read((char*) &pack.v.x, sizeof(float));
        ifs.read((char*) &pack.v.y, sizeof(float));
        ifs.read((char*) &pack.v.z, sizeof(float));
        mesh.append(pack.v, stream::type::vertex);

        ifs.read((char*) &pack.t.x, sizeof(float));
        ifs.read((char*) &pack.t.y, sizeof(float));
        ifs.read((char*) &pack.t.z, sizeof(float));
        mesh.append(pack.t, stream::type::vertex);

        ifs.read((char*) &pack.n.y, sizeof(float));
        ifs.read((char*) &pack.n.y, sizeof(float));
        ifs.read((char*) &pack.n.y, sizeof(float));
        mesh.append(pack.n, stream::type::vertex);
    }

    ifs.close();
    return false;
}

bool streamparser::load_mesh(stream::mesh &mesh, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        return util::log("Failed to load object because there is no path insert");
    }

    mesh.format = mesh.format != stream::format::unknown ? mesh.format : this->get_model_format(path);
    this->current_path = path;

    switch (mesh.format) {
    case stream::format::wavefrontobj:
        return this->process_wavefront_object(mesh);
    case stream::format::stl:
        return this->process_stl(mesh);
    case stream::format::gltf:
        return util::log("Not implemented!");
    case stream::format::unknown:
        return util::log("Unknown model format, please try: wavefrontobj, stl, or gltf");
    }

    return true;
}

stream::serializer streamparser::get_pbr_from_wavefront_object_mtllib(stream::serializer &serializer) {
    auto &mtllib_metadata {serializer.get_metadata()};
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

bool streamparser::process_wavefront_object_mtllib(stream::mtl &mtl) {
    std::ifstream ifs_wavefront_object {this->current_path.data(), std::ifstream::in};
    if (!ifs_wavefront_object.is_open()) {
        return util::log("Failed to read wavefront object at '" + this->current_path + '\'');
    }

    std::string string_buffer {};
    std::string substr {};
    std::vector<std::string> strings {};
    std::strng mtllib_path {};

    while (std::getline(ifs_wavefront_object, string_buffer)) {
        if (string_buffer.size() < 6) {
            continue;
        }

        substr = string_buffer.substr(0, 6);
        if (substr == "mtllib") {
            util::split(strings, this->current_path, '/');

            const uint64_t size {strings[std::max((int32_t) strings.size() - 1, 0)].size()};
            const uint64_t tab_find {find.size()};
            
            const std::string folder {this->current_path.substr(0, this->current_path.size() - size)};
            const std::string file {string_buffer.substr(tab_find, string_buffer.size() - tab_find)};
            mtllib_path = folder + file;
            continue;
        }
    }

    ifs_wavefront_object.close();
    std::ifstream ifs_mtllib {mtllib_path.c_str(), std::ifstream::in};
    if (!ifs_mtllib.is_open()) {
        return util::log("Failed to read material from wavefront object at '" + mtllib_path + '\'');
    }

    std::string newmtl_segment {};
    std::string metadata {};

    constexpr uint32_t x {1};
    constexpr uint32_t y {2};
    constexpr uint32_t z {3};

    while (std::getline(ifs_mtllib, string_buffer)) {
        if (string_buffer.size() < 2) {
            continue;
        }

        if (string_buffer[0] == '\t') {
            string_buffer = string_buffer.substr(1, string_buffer.size() - 1);
        }

        util::split(strings, string_buffer, ' ');

        if (strings[0] == "illum" || strings[0] == "Ns" || strings[0] == "Ni" || strings[0] == "d" || strings[0] == "Tr") {
            this->current_serialiazer[newmtl_segment][strings[0]] = strings[x];
        } else if (strings[0] == "newmtl") {
            metadata.clear();
            metadata += string_buffer.substr(strings[0].size() + 1, string_buffer.size() - strings[0].size() - 1);
            this->current_serialiazer[metadata];
        } else if (strings[0] == "Ka" || strings[0] == "Kd" || strings[0] == "Ks" || strings[0] == "Ke" || strings[0] == "Tf") {
            metadata.clear();
            metadata += strings[x];
            metadata += '/';
            metadata += strings[y];
            metadata += '/';
            metadata += strings[z];
            this->current_serialiazer[newmtl_segment][strings[0]] = metadata;
        } else if (strings[0] == "bump" || strings[0] == "map_bump" || strings[0] == "map_Ka" || strings[0] == "map_Kd" || strings[0] == "map_Ks" || strings[0] == "map_Ke") {
            metadata.clear();
            metadata += string_buffer.substr(strings[0].size() + 1, string_buffer.size() - strings[0].size() - 1);
            this->current_serialiazer[newmtl_segment][strings[0]] = metadata;
        }
    }

    ifs_mtllib.close();
    mtl.set_serialiazer(this->get_pbr_from_wavefront_object_mtllib(this->current_serialiazer));
}

bool streamparser::load_mtl(stream::mtl &mtl, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        return util::log("Failed to load material from the model because there is no path insert");
    }

    mesh.format = mtl.format != stream::format::unknown ? mtl.format ? this->get_model_format(path);
    this->current_path = path;

    switch (mtl.format) {
    case stream::format::wavefrontobj:
        return this->process_wavefront_object_mtllib(mtl);
    case stream::format::stl:
        return !util::log("STL does not contains any material file");
    case stream::format::gltf:
        return util::log("Not implemented!");
    case stream::format::unknown:
        return util::log("Unknown model format, pleasetry: wavefrontobj, stl, or gltf");
    }

    return true;
}