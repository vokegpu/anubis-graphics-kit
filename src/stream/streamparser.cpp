#include "streamparser.hpp"
#include <nlohmann/json.hpp>
#include "agk.hpp"

int32_t streamparser::gltexturefilter[2] {};
int32_t streamparser::gltexturewrap[2] {};

stream::format streamparser::get_model_format(std::string_view path) {
    std::vector<std::string> strings {};
    util::split(strings, path, '.');
    strings.emplace_back();

    const std::string file_extension {strings[std::max((int32_t) strings.size() - 2, 0)]};
    const stream::format format {!this->mesh_ext_map.count(file_extension) ? stream::format::unknown : this->mesh_ext_map[file_extension]};
    return format;
}

bool streamparser::read_wavefront_object_mtllib_index(std::ifstream &ifs_wavefront_object) {
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
            const uint64_t tab_find {find.size() + 1};
            
            const std::string folder {this->current_path.substr(0, this->current_path.size() - size)};
            const std::string file {string_buffer.substr(tab_find, string_buffer.size() - tab_find)};
            mtllib_path = folder + file;
            break;
        }
    }

    std::ifstream ifs_mtllib {mtllib_path.c_str()};
    if (!ifs_mtllib.is_open()) {
        return util::log("Failed to index materials from wavefront object at '" + mtllib_path + '\'');
    }

    uint64_t index {};
    while (std::getline(ifs_mtllib, string_buffer)) {
        if (string_buffer.size() < 2) {
            continue;
        }

        if (string_buffer[0] == '\t') {
            string_buffer = string_buffer.substr(1, string_buffer.size() - 1);
        }

        util::split(strings, string_buffer, ' ');
        if (strings[0] == "newmtl") {
            this->wavefront_object_material_map[strings[1]] = index++;
        }
    }

    ifs_mtllib.close();
    return false;
}

bool streamparser::process_wavefront_object_meshes(std::vector<stream::mesh> &meshes) {
    std::ifstream ifs {this->current_path.data()};
    if (!ifs.is_open()) {
        return util::log("Failed to read wavefront object at '" + this->current_path + '\'');
    }

    this->read_wavefront_object_mtllib_index(ifs);
    std::string model_filename {};
    this->read_mesh_filename(model_filename, this->current_path);

    std::string string_buffer {};
    std::string find {};
    std::string values {};
    std::vector<std::string> split {}, split_first {}, split_second {}, split_third {};

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
    uint64_t mesh_index {};
    bool clean_mesh_pack {};

    while (std::getline(ifs, string_buffer)) {
        if ((string_buffer.size() < 1 || ((!(find = string_buffer.substr(0, 2)).empty() || true) && find != "v " && find != "vt" && find != "vn" && find != "f ")) && (string_buffer.size() < 6 || (find = string_buffer.substr(0, 6)) != "usemtl")) {
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

        if (find == "usemtl") {
            if (!meshes.empty()) {
                mesh_index++;
            }

            meshes.emplace_back();
            meshes[mesh_index].material = this->wavefront_object_material_map[split[1]];
            meshes[mesh_index] += mesh_index > 0 ? meshes[mesh_index-1] : meshes[mesh_index];
            meshes[mesh_index].tag = std::to_string(mesh_index);
        } else if (find == "v ") {
            if (clean_mesh_pack) {
                pack = {};
                mesh_index++;
                clean_mesh_pack = false;

                meshes.emplace_back();
                meshes[mesh_index].tag = std::to_string(mesh_index);
            }

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
            clean_mesh_pack = true;

            for (int32_t indexes {1}; indexes < split.size(); indexes++) {
                util::split(split_first, split[indexes], '/');
                meshes[mesh_index].faces++;

                if (v_contains) {
                    vsize = (uint32_t) pack.v.size() + 1;
                    index = (std::stoi(split_first[0]));
                    index = (index + vsize) % vsize;

                    meshes[mesh_index].append(pack.v[index - 1], stream::type::vertex);
                }

                if (t_contains) {
                    vsize = (uint32_t) pack.t.size() + 1;
                    index = (std::stoi(split_first[1]));
                    index = (index + vsize) % vsize;

                    meshes[mesh_index].append(pack.t[index - 1], stream::type::texcoord);
                }

                if (n_contains) {
                    vsize = (uint32_t) pack.n.size() + 1;
                    index = (std::stoi(split_first[2 - (!t_contains)]));
                    index = (index + vsize) % vsize;

                    meshes[mesh_index].append(pack.n[index - 1], stream::type::normal);
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
    mesh.tag = "0";

    /*
     * triangle vertices are represented as: t, r, & s
     * face normals are represented by n
     */
    for (int32_t it {}; it < mesh.faces; it++) {
        ifs.read(reinterpret_cast<char*>(&n.x), 4);
        ifs.read(reinterpret_cast<char*>(&n.y), 4);
        ifs.read(reinterpret_cast<char*>(&n.z), 4);

        ifs.read(reinterpret_cast<char*>(&t.x), 4);
        ifs.read(reinterpret_cast<char*>(&t.y), 4);
        ifs.read(reinterpret_cast<char*>(&t.z), 4);

        mesh.append(t, stream::type::vertex);
        mesh.append(n, stream::type::normal);

        ifs.read(reinterpret_cast<char*>(&r.x), 4);
        ifs.read(reinterpret_cast<char*>(&r.y), 4);
        ifs.read(reinterpret_cast<char*>(&r.z), 4);
        mesh.append(r, stream::type::vertex);
        mesh.append(n, stream::type::normal);

        ifs.read(reinterpret_cast<char*>(&s.x), 4);
        ifs.read(reinterpret_cast<char*>(&s.y), 4);
        ifs.read(reinterpret_cast<char*>(&s.z), 4);

        mesh.append(s, stream::type::vertex);
        mesh.append(n, stream::type::normal);

        ifs.read(buf, 2);
    }

    ifs.close();
    return false;
}

uint64_t streamparser::get_gltf_component_size(uint32_t n) {
    switch (n) {
    case 5120:
    case 5121:
        return 1;
    case 5122:
    case 5123:
        return 2;
    case 5125:
    case 5126:
        return 4;
    }

    return 0;
}

bool streamparser::read_gltf_mesh_bytes(stream::mesh &mesh, stream::type mesh_type, nlohmann::json &value) {
    auto &accessor = this->current_gltf_accessors_json.at(value.get<uint64_t>());
    auto &buffer_view = this->current_gltf_buffer_views_json.at(accessor["bufferView"].get<uint64_t>());
    auto &ifs {this->current_gltf_ifs_binary.at(buffer_view["buffer"].get<uint64_t>())};

    if (!ifs.is_open()) {
        return util::log("Failed to read mesh bytes from glTF binary because the binary is not open");
    }

    const uint64_t offset {buffer_view["byteOffset"].get<uint64_t>()};
    const uint64_t length {buffer_view["byteLength"].get<uint64_t>()};
    const uint64_t component_size {this->get_gltf_component_size(accessor["componentType"].get<uint32_t>())};
    const uint64_t accessor_offset {accessor["byteOffset"].is_null() ? 0 : accessor["byteOffset"].get<uint64_t>()};
    const uint64_t vec_size {mesh_type == stream::type::index ? (uint64_t) 1 : (mesh_type == stream::type::texcoord ? (uint64_t) 2 : (uint64_t) 3)};
    const uint64_t byte_iterations {accessor["count"].get<uint64_t>()};

    ifs.seekg(offset + accessor_offset);
    uint32_t i {};
    float f {};

    glm::vec3 v3 {};
    glm::vec2 v2 {};

    for (uint64_t it {}; it < byte_iterations; it++) {
        switch (vec_size) {
        case 1:
            ifs.read(reinterpret_cast<char*>(&i), component_size);
            mesh.append(i, mesh_type);
            break;
        case 2:
            ifs.read(reinterpret_cast<char*>(&v2.x), component_size);
            ifs.read(reinterpret_cast<char*>(&v2.y), component_size);

            v2.y = 1.0f - v2.y;
            mesh.append(v2, mesh_type);
            break;
        default:
            ifs.read(reinterpret_cast<char*>(&v3.x), component_size);
            ifs.read(reinterpret_cast<char*>(&v3.y), component_size);
            ifs.read(reinterpret_cast<char*>(&v3.z), component_size);
            mesh.append(v3, mesh_type);
            break;
        }
    }

    return false;
}

bool streamparser::process_gltf(stream::mesh &mesh, nlohmann::json &gltf_node) {
    std::string key {};
    bool process_accessor {};
    stream::type streaming_mesh_type {};

    auto &primitives = gltf_node["primitives"].at(0);
    for (auto &itp : primitives.items()) {
        key = itp.key();
        if (key == "attributes") {
            for (auto &ita : itp.value().items()) {
                key = ita.key();
                process_accessor = key == "POSITION" || key == "NORMAL";
                if (process_accessor) {
                    streaming_mesh_type = this->gltf_primitive_conversions_map[key];
                } else {
                    std::string texcoord_attrib {};
                    for (uint32_t it {}; it < 24; it++) {
                        texcoord_attrib = "TEXCOORD_" + std::to_string(it);
                        if (key == texcoord_attrib) {
                            process_accessor = true;
                            streaming_mesh_type = stream::type::texcoord;
                            break;
                        }
                    }
                }

                if (process_accessor) {
                    this->read_gltf_mesh_bytes(mesh, streaming_mesh_type, ita.value());
                }
            }
        } else if (key == "indices") {
            this->read_gltf_mesh_bytes(mesh, stream::type::index, itp.value());
        } else if (key == "material") {
            mesh.material = itp.value().get<uint32_t>();
        }
    }

    return false;
}

bool streamparser::process_gltf_mtl(stream::mtl &mtl) {
    std::ifstream ifs {this->current_path};
    if (!ifs.is_open()) {
        return util::log("Failed to read glTF at '" + this->current_path + '\'');
    }

    this->current_gltf_json = nlohmann::json::parse(ifs);
    std::string model_filename {};
    this->read_mesh_filename(model_filename, this->current_path);

    auto &materials = this->current_gltf_json["materials"];
    auto &samplers = this->current_gltf_json["samplers"];
    auto &images = this->current_gltf_json["images"];
    auto &textures = this->current_gltf_json["textures"];

    std::string image_uri {};
    std::string asset_tag {};
    std::vector<std::string> strings {};

    uint32_t gl_texture_format[2] {};
    nlohmann::json sampler {};
    std::map<uint64_t, std::string> asset_map {};
    uint64_t texture_index {};

    for (nlohmann::json &texture : textures) {
        if (texture.is_null() || texture["source"].is_null()) {
            continue;
        }

        image_uri = images.at(texture["source"].get<uint64_t>())["uri"].get<std::string>();

        /*
         * Check internal format and format based at image file extension.
         * BMP or any other extension is by default GL_RGB.
         */
        gl_texture_format[0] = GL_RGB;
        gl_texture_format[1] = GL_RGB;

        util::split(strings, image_uri, '.');
        if (!strings.empty() && strings[strings.size() - 1] == "png") {
            gl_texture_format[0] = GL_RGB;
            gl_texture_format[1] = GL_RGB;
        } else if (!strings.empty() && strings[strings.size() - 1] == "jpg") {
            gl_texture_format[0] = GL_RGB;
            gl_texture_format[1] = GL_RGB;
        }

        streamparser::gltexturefilter[0] = GL_NEAREST;
        streamparser::gltexturefilter[1] = GL_NEAREST;

        streamparser::gltexturewrap[0] = GL_REPEAT;
        streamparser::gltexturewrap[1] = GL_REPEAT;

        if (!texture["sampler"].is_null()) {
            sampler = samplers.at(texture["sampler"].get<uint64_t>());
            if (!sampler["minFilter"].is_null()) {
                switch (sampler["minFilter"].get<int32_t>()) {
                case 9729:
                    streamparser::gltexturefilter[0] = GL_LINEAR;
                    break;
                case 9984:
                    streamparser::gltexturefilter[0] = GL_NEAREST_MIPMAP_NEAREST;
                    break;
                case 9985:
                    streamparser::gltexturefilter[0] = GL_LINEAR_MIPMAP_NEAREST;
                    break;
                case 9986:
                    streamparser::gltexturefilter[0] = GL_NEAREST_MIPMAP_LINEAR;
                    break;
                case 9987:
                    streamparser::gltexturefilter[0] = GL_LINEAR_MIPMAP_LINEAR;
                    break;
                }
            }

            if (!sampler["magFilter"].is_null()) {
                switch (sampler["magFilter"].get<int32_t>()) {
                case 9729:
                    streamparser::gltexturefilter[1] = GL_LINEAR;
                    break;
                case 9984:
                    streamparser::gltexturefilter[1] = GL_NEAREST_MIPMAP_NEAREST;
                    break;
                case 9985:
                    streamparser::gltexturefilter[1] = GL_LINEAR_MIPMAP_NEAREST;
                    break;
                case 9986:
                    streamparser::gltexturefilter[1] = GL_NEAREST_MIPMAP_LINEAR;
                    break;
                case 9987:
                    streamparser::gltexturefilter[1] = GL_LINEAR_MIPMAP_LINEAR;
                    break;
                }
            }

            if (!sampler["wrapS"].is_null()) {
                switch (sampler["wrapS"].get<int32_t>()) {
                case 33071:
                    streamparser::gltexturewrap[0] = GL_CLAMP_TO_EDGE;
                    break;
                case 33648:
                    streamparser::gltexturewrap[0] = GL_MIRRORED_REPEAT;
                    break;
                }
            }

            if (!sampler["wrapT"].is_null()) {
                switch (sampler["wrapT"].get<int32_t>()) {
                case 33071:
                    streamparser::gltexturewrap[1] = GL_CLAMP_TO_EDGE;
                    break;
                case 33648:
                    streamparser::gltexturewrap[1] = GL_MIRRORED_REPEAT;
                    break;
                }
            }
        }

        this->read_full_filename(image_uri, image_uri);
        asset_tag = model_filename + "." + image_uri;
        image_uri = AGK_DEFAULT_DIR_TEXTURES + image_uri;

        auto *p_asset {new asset::texture<uint8_t> {asset_tag, image_uri, {GL_TEXTURE_2D, GL_UNSIGNED_BYTE, gl_texture_format[0], gl_texture_format[1]}, [](gpu::texture &texture, bool &mipmap) {
            glTextureParameteri(texture.type, GL_TEXTURE_MIN_FILTER, streamparser::gltexturefilter[0]);
            glTextureParameteri(texture.type, GL_TEXTURE_MAG_FILTER, streamparser::gltexturefilter[1]);

            glTextureParameteri(texture.type, GL_TEXTURE_WRAP_S, streamparser::gltexturewrap[0]);
            glTextureParameteri(texture.type, GL_TEXTURE_WRAP_T, streamparser::gltexturewrap[1]);

            mipmap = true;
        }}};

        agk::asset::load(p_asset);
        asset_map[texture_index] = p_asset->tag;
        texture_index++;
    }

    stream::serializer &serializer {mtl.get_serializer()};
    nlohmann::json pbr_metallic_roughness {};
    std::string color_factor {};

    uint32_t index {};
    for (nlohmann::json &material : materials) {
        pbr_metallic_roughness = material["pbrMetallicRoughness"];
        if (material.is_null() || pbr_metallic_roughness.is_null()) {
            continue;
        }

        const std::string name {std::to_string(index)};
        index++;

        serializer[name]["doubleSided"] = material["doubleSided"].is_null() ? "1" : stream::i(material["doubleSided"].get<bool>());
        serializer[name]["metal"] = stream::f(pbr_metallic_roughness["metallicFactor"].get<float>());
        serializer[name]["rough"] = pbr_metallic_roughness["roughnessFactor"].is_null() ? "0.72" : stream::f(pbr_metallic_roughness["roughnessFactor"].get<float>());

        if (pbr_metallic_roughness["baseColorFactor"].is_array()) {
            color_factor.clear();
            for (nlohmann::json &value : pbr_metallic_roughness["baseColorFactor"]) {
                color_factor += stream::f(value.get<float>());
                color_factor += ' ';
            }

            serializer[name]["color"] = color_factor.substr(0, color_factor.size() - 1); // remove last space char;
            serializer[name]["albedo"].clear();
        } else if (pbr_metallic_roughness["baseColorTexture"].is_object()) {
            serializer[name]["color"] = "1.0 1.0 1.0";
            serializer[name]["albedo"] = asset_map[pbr_metallic_roughness["baseColorTexture"]["index"].get<uint64_t>()];
        }
    }

    ifs.close();
    return false;
}

bool streamparser::process_gltf_meshes(std::vector<stream::mesh> &meshes) {
    std::ifstream ifs {this->current_path.data()};
    if (!ifs.is_open()) {
        return util::log("Failed to read glTF at '" + this->current_path + '\'');
    }

    this->current_gltf_json = nlohmann::json::parse(ifs);
    this->current_gltf_meshes_json = this->current_gltf_json["meshes"];
    this->current_gltf_accessors_json = this->current_gltf_json["accessors"];
    this->current_gltf_buffer_views_json = this->current_gltf_json["bufferViews"];

    std::vector<std::string> strings {};
    util::split(strings, this->current_path, '/');

    const uint64_t size {strings[std::max((int32_t) strings.size() - 1, 0)].size()};
    const std::string folder {this->current_path.substr(0, this->current_path.size() - size)};
    std::string binary_path {};

    for (nlohmann::json &buffer : this->current_gltf_json["buffers"]) {
        auto &binary_ifs {this->current_gltf_ifs_binary.emplace_back()};
        binary_path = folder + buffer["uri"].get<std::string>();
        binary_ifs.open(binary_path.c_str(), std::ifstream::binary);
    }

    /*
     * -> read
     * Mesh tag is a number and not the node name,
     * because the dev who loads the glTF do not
     * know the nodes name.
     */
    auto &nodes = this->current_gltf_json["nodes"];
    uint32_t node_count {};

    for (nlohmann::json &node : nodes) {
        if (node.is_null() || node["mesh"].is_null()) {
            continue;
        }

        stream::mesh &mesh {meshes.emplace_back()};
        mesh.format = stream::format::gltf;
        mesh.tag = std::to_string(node_count);

        uint64_t mesh_index {node["mesh"].get<uint64_t>()};
        this->process_gltf(mesh, this->current_gltf_meshes_json.at(mesh_index));
        node_count++;
    }

    ifs.close();
    for (std::ifstream &ifs_binary : this->current_gltf_ifs_binary) {
        if (ifs_binary.is_open()) {
            ifs_binary.close();
        }
    }

    this->current_gltf_ifs_binary.clear();
    return false;
}

bool streamparser::load_meshes(std::vector<stream::mesh> &meshes, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        return util::log("Failed to load object because there is no path insert");
    }

    this->current_path = path;
    stream::format format {stream::format {this->get_model_format(path)}};

    switch (format) {
    case stream::format::wavefrontobj:
        return this->process_wavefront_object_meshes(meshes);
    case stream::format::stl:
        return this->process_stl(meshes.emplace_back());
    case stream::format::gltf:
        return this->process_gltf_meshes(meshes);
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
    auto &serializer {mtl.get_serializer()};

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
            serializer[newmtl_segment][strings[0]] = strings[x];
        } else if (strings[0] == "newmtl") {
            metadata.clear();
            metadata += string_buffer.substr(strings[0].size() + 1, string_buffer.size() - strings[0].size() - 1);
            serializer[metadata];
        } else if (strings[0] == "Ka" || strings[0] == "Kd" || strings[0] == "Ks" || strings[0] == "Ke" || strings[0] == "Tf") {
            metadata.clear();
            metadata += strings[x];
            metadata += '/';
            metadata += strings[y];
            metadata += '/';
            metadata += strings[z];
            serializer[newmtl_segment][strings[0]] = metadata;
        } else if (strings[0] == "bump" || strings[0] == "map_bump" || strings[0] == "map_Ka" || strings[0] == "map_Kd" || strings[0] == "map_Ks" || strings[0] == "map_Ke") {
            metadata.clear();
            metadata += string_buffer.substr(strings[0].size() + 1, string_buffer.size() - strings[0].size() - 1);
            serializer[newmtl_segment][strings[0]] = metadata;
        }
    }

    ifs_mtllib.close();
    mtl.set_serializer(this->get_pbr_from_wavefront_object_mtllib(serializer));
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
        return this->process_gltf_mtl(mtl);
    case stream::format::unknown:
        return util::log("Unknown model format, please try: *.obj *.stl *.gltf");
    }

    return true;
}

bool streamparser::read_full_filename(std::string &filename, std::string_view path) {
    if (path.empty() || path.size() < 3) {
        return util::log("Failed to read model filename because there is no path insert");
    }

    std::vector<std::string> strings {};
    util::split(strings, path, '.');

    if (strings.size() <= 1 || strings.empty()) {
        return util::log(R"(Empty filename ".*", please rename to: "*.*")");
    }

    filename = strings[strings.size() - 2] + "." + strings[strings.size() - 1];
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