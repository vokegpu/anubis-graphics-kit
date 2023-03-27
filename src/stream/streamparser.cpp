#include "streamparser.hpp"
#include <nlohmann/json.hpp>

stream::format streamparser::get_model_format(std::string_view path) {
    std::vector<std::string> strings {};
    util::split(strings, path, '.');
    strings.emplace_back();

    const std::string file_extension {strings[std::max((int32_t) strings.size() - 2, 0)]};
    const stream::format format {!this->mesh_ext_map.count(file_extension) ? stream::format::unknown : this->mesh_ext_map[file_extension]};
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

    size_t line_size {};
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
    std::ifstream ifs {this->current_path.data(), std::ifstream::in | std::ifstream::binary};
    if (!ifs.is_open()) {
        return util::log("Failed to read STL at '" + this->current_path + '\'');
    }

    ifs.seekg(80);
    ifs.read((char*) &mesh.faces, sizeof(int32_t));
    stream::mesh::pack pack {};
    glm::vec3 t {}, r {}, s {}, n {};
    char buf[2] {};

    /*
     * triangle vertices are represented as: t, r, & s
     * face normals are represented by n
     */
    for (int32_t it {}; it < mesh.faces; it++) {
        ifs.read((char*) &n.x, sizeof(float));
        ifs.read((char*) &n.y, sizeof(float));
        ifs.read((char*) &n.z, sizeof(float));
        mesh.append(n, stream::type::normal);
        mesh.append(n, stream::type::normal);
        mesh.append(n, stream::type::normal);

        ifs.read((char*) &t.x, sizeof(float));
        ifs.read((char*) &t.y, sizeof(float));
        ifs.read((char*) &t.z, sizeof(float));
        mesh.append(t, stream::type::vertex);

        ifs.read((char*) &r.x, sizeof(float));
        ifs.read((char*) &r.y, sizeof(float));
        ifs.read((char*) &r.z, sizeof(float));
        mesh.append(r, stream::type::vertex);

        ifs.read((char*) &s.x, sizeof(float));
        ifs.read((char*) &s.y, sizeof(float));
        ifs.read((char*) &s.z, sizeof(float));
        mesh.append(s, stream::type::vertex);
        ifs.read(buf, 2);
    }

    ifs.close();
    return false;
}

uint64_t streamparser::get_gltf_component_size(uint32_t n) {
    switch (n) {
    case 5120:
        return sizeof(int8_t);
    case 5121:
        return sizeof(uint8_t);
    case 5122:
        return sizeof(int16_t);
    case 5123:
        return sizeof(uint16_t);
    case 5125:
        return sizeof(int32_t);
    case 5126:
        return sizeof(uint32_t);
    }

    return sizeof(nullptr);
}

bool streamparser::read_gltf_mesh_bytes(stream::mesh *p_mesh, stream::type mesh_type, std::ifstream &ifs, uint64_t offset, uint64_t length, uint32_t count, uint64_t component_size) {
    ifs.seekg(offset);
    float n {};
    uint32_t vector_len {};

    switch (mesh_type) {
    case stream::type::texcoord:
        vector_len = 2;
        break;
    case stream::type::index:
        vector_len = 1;
        break;
    default:
        vector_len = 3;
        break;
    }

    for (uint32_t it {}; it < count*vector_len; it++) {
        ifs.read((char*) &n, component_size);
        if (mesh_type == stream::type::index && n > 0.0f) {
            p_mesh->append(n, mesh_type);
        } else {
            p_mesh->append(n, mesh_type);
        }
    }

    return false;
}

bool streamparser::process_gltf(stream::mesh *p_mesh, stream::mtl *p_mtl, nlohmann::json &gltf_node) {
    auto &primitives = gltf_node["primitives"].at(0);
    bool mesh_streaming_serialize_task {p_mesh != nullptr};
    bool mtl_streaming_serialize_task {p_mtl != nullptr};

    if (!(mesh_streaming_serialize_task || mtl_streaming_serialize_task)) {
        return util::log("Invalid input stream: null");
    }

    std::string key {};
    bool process_accessor {};
    stream::type streaming_meshs_type {};

    for (auto &itp : primitives.items()) {
        key = itp.key();
        if (key == "attributes" && mesh_streaming_serialize_task) {
            for (auto &ita : itp.value().items()) {
                key = ita.key();
                process_accessor = key == "POSITION" || key == "NORMAL";
                if (process_accessor) {
                    streaming_meshs_type = this->gltf_primitive_conversions_map[key];
                } else {
                    std::string texcoord_attrib {};
                    for (uint32_t it {}; it < 24; it++) {
                        texcoord_attrib = "TEXCOORD_" + std::to_string(it);
                        if (key == texcoord_attrib) {
                            process_accessor = true;
                            streaming_meshs_type = stream::type::texcoord;
                            break;
                        }
                    }
                }

                if (process_accessor) {
                    auto &accessor = this->current_gltf_accessors_json.at(ita.value().get<uint64_t>());
                    auto &buffer_view = this->current_gltf_buffer_views_json.at(accessor["bufferView"].get<uint64_t>());
                    uint32_t count {accessor["count"].get<uint32_t>()};

                    const uint64_t offset {buffer_view["byteOffset"].get<uint64_t>()};
                    const uint64_t length {buffer_view["byteLength"].get<uint64_t>()};
                    auto &ifs {this->current_gltf_ifs_binary.at(buffer_view["buffer"].get<uint64_t>())};

                    util::log("Found accessor byte " + key + " " + std::to_string(offset));
                    this->read_gltf_mesh_bytes(p_mesh, streaming_meshs_type, ifs, offset, length, count, this->get_gltf_component_size(accessor["componentType"].get<uint32_t>()));
                }
            }
        } else if (key == "indices" && mesh_streaming_serialize_task) {
            auto &accessor = this->current_gltf_accessors_json.at(itp.value().get<uint64_t>());
            auto &buffer_view = this->current_gltf_buffer_views_json.at(accessor["bufferView"].get<uint64_t>());
            uint32_t count {accessor["count"].get<uint32_t>()};

            const uint64_t offset {buffer_view["byteOffset"].get<uint64_t>()};
            const uint64_t length {buffer_view["byteLength"].get<uint64_t>()};
            auto &ifs {this->current_gltf_ifs_binary.at(buffer_view["buffer"].get<uint64_t>())};

            util::log("Found accessor byte " + key + " " + std::to_string(offset));
            this->read_gltf_mesh_bytes(p_mesh, stream::type::index, ifs, offset, length, count, this->get_gltf_component_size(accessor["componentType"].get<uint32_t>()));
        } else if (key == "material" && mtl_streaming_serialize_task) {
            // @TODO material checking & streaming
        }
    }

    return false;
}

bool streamparser::read_gltf_json_modules() {
    std::ifstream ifs {this->current_path.data()};
    if (!ifs.is_open()) {
        return util::log("Failed to glTF at '" + this->current_path + '\'');
    }

    this->current_gltf_json = nlohmann::json::parse(ifs);
    this->current_gltf_meshes_json = this->current_gltf_json["meshes"];
    this->current_gltf_accessors_json = this->current_gltf_json["accessors"];
    this->current_gltf_buffer_views_json = this->current_gltf_json["bufferViews"];
    return false;
}

bool streamparser::load_gltf_meshes(std::vector<stream::mesh> &meshes, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        return util::log("Failed to load glTF meshes because there is no path insert");
    }

    stream::format format {this->get_model_format(path)};
    if (format != stream::format::gltf) {
        return util::log("Incompatible file extension");
    }

    std::ifstream ifs {path.data()};
    if (!ifs.is_open()) {
        return util::log("Failed to read glTF at '" + this->current_path + '\'');
    }

    this->current_path = path;
    this->read_gltf_json_modules();

    std::vector<std::string> strings {};
    util::split(strings, this->current_path, '/');

    const uint64_t size {strings[std::max((int32_t) strings.size() - 1, 0)].size()};
    const std::string folder {this->current_path.substr(0, this->current_path.size() - size)};
    std::string binary_path {};

    for (nlohmann::json &buffer : this->current_gltf_json["buffers"]) {
        auto &binary_ifs {this->current_gltf_ifs_binary.emplace_back()};
        binary_path = folder + buffer["uri"].get<std::string>();
        binary_ifs.open(binary_path.c_str(), std::ifstream::in | std::ifstream::binary);
    }

    /*
     * -> read
     * Mesh tag is a number and not the node name,
     * because the dev who loads the glTF do not
     * know the nodes name.
     */
    auto &nodes = this->current_gltf_json["nodes"];
    for (uint64_t it {}; it < nodes.size(); it++) {
        stream::mesh &mesh {meshes.emplace_back()};
        mesh.format = format;
        mesh.tag = std::to_string(it);

        uint64_t mesh_index {nodes.at(it)["mesh"].get<uint64_t>()};
        this->process_gltf(&mesh, nullptr, this->current_gltf_meshes_json.at(mesh_index));
    }

    ifs.close();
    for (std::ifstream &ifs_binary : this->current_gltf_ifs_binary) {
        if (ifs_binary.is_open()) {
            ifs_binary.close();
        }
    }

    return false;
}

bool streamparser::load_mesh(stream::mesh &mesh, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        return util::log("Failed to load object because there is no path insert");
    }

    mesh.format = mesh.format != stream::format::unknown ? mesh.format : this->get_model_format(path);
    this->current_path = path;
    this->read_mesh_filename(mesh.tag, path);

    switch (mesh.format) {
    case stream::format::wavefrontobj:
        return this->process_wavefront_object(mesh);
    case stream::format::stl:
        return this->process_stl(mesh);
    case stream::format::gltf:
        return util::log("Incompatible file extension (*.gltf) with this method");
    case stream::format::unknown:
        return util::log("Unknown model format, please try: *.obj *.stl *.gltf");
    }

    return true;
}

stream::serializer streamparser::get_pbr_from_wavefront_object_mtllib(stream::serializer &serializer) {
    auto &mtllib_metadata {serializer.get_metadata()};
    stream::serializer pbr_mtl_serializer {};
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
    std::string find {};
    std::vector<std::string> strings {};
    std::string mtllib_path {};

    while (std::getline(ifs_wavefront_object, string_buffer)) {
        if (string_buffer.size() < 6) {
            continue;
        }

        find = string_buffer.substr(0, 6);
        if (find == "mtllib") {
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
            this->current_serializer[newmtl_segment][strings[0]] = strings[x];
        } else if (strings[0] == "newmtl") {
            metadata.clear();
            metadata += string_buffer.substr(strings[0].size() + 1, string_buffer.size() - strings[0].size() - 1);
            this->current_serializer[metadata];
        } else if (strings[0] == "Ka" || strings[0] == "Kd" || strings[0] == "Ks" || strings[0] == "Ke" || strings[0] == "Tf") {
            metadata.clear();
            metadata += strings[x];
            metadata += '/';
            metadata += strings[y];
            metadata += '/';
            metadata += strings[z];
            this->current_serializer[newmtl_segment][strings[0]] = metadata;
        } else if (strings[0] == "bump" || strings[0] == "map_bump" || strings[0] == "map_Ka" || strings[0] == "map_Kd" || strings[0] == "map_Ks" || strings[0] == "map_Ke") {
            metadata.clear();
            metadata += string_buffer.substr(strings[0].size() + 1, string_buffer.size() - strings[0].size() - 1);
            this->current_serializer[newmtl_segment][strings[0]] = metadata;
        }
    }

    ifs_mtllib.close();
    mtl.set_serializer(this->get_pbr_from_wavefront_object_mtllib(this->current_serializer));
    return false;
}

bool streamparser::load_mtl(stream::mtl &mtl, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        return util::log("Failed to load material from the model because there is no path insert");
    }

    mtl.format = mtl.format != stream::format::unknown ? mtl.format : this->get_model_format(path);
    this->current_path = path;

    switch (mtl.format) {
    case stream::format::wavefrontobj:
        return this->process_wavefront_object_mtllib(mtl);
    case stream::format::stl:
        return !util::log("STL does not contains any material file");
    case stream::format::gltf:
        return util::log("Not implemented!");
    case stream::format::unknown:
        return util::log("Unknown model format, please try: *.obj *.stl *.gltf");
    }

    return true;
}

bool streamparser::read_mesh_filename(std::string &filename, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        return util::log("Failed to read model filename because there is no path insert");
    }

    std::vector<std::string> strings {};
    util::split(strings, path, '.');

    if (strings.size() <= 1 || strings.empty()) {
        return util::log(R"(Empty filename ".*", please rename to: "*.*")");
    }

    filename = strings[strings.size() - 2];
    int64_t it {};

    for (it = filename.size(); it > 0; --it) {
        if (filename[it] == '\\' || filename[it] == '/') {
            it++;
            break;
        }
    }

    filename = filename.substr(it, filename.size());
    return false;
}