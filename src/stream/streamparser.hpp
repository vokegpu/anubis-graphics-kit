#ifndef AGK_STREAM_PARSER_H
#define AGK_STREAM_PARSER_H

#include "stream.hpp"
#include "asset/texture.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

class streamparser {
private:
    static int32_t gltexturewrap[2];;
    static int32_t gltexturefilter[2];
protected:
    std::string current_path {};
    std::vector<std::ifstream> current_gltf_ifs_binary {};

    nlohmann::json current_gltf_json {};
    nlohmann::json current_gltf_meshes_json {};
    nlohmann::json current_gltf_accessors_json {};
    nlohmann::json current_gltf_buffer_views_json {};

    std::map<std::string, stream::format> mesh_ext_map {
            {"stl", stream::format::stl}, {"obj", stream::format::wavefrontobj}, {"gltf", stream::format::gltf}
    };

    std::map<std::string, stream::type> gltf_primitive_conversions_map {
            {"POSITION", stream::type::vertex}, {"indices", stream::type::index}, {"NORMAL", stream::type::normal}
    };

    std::map<std::string, std::string> mtl_pbr_conversions_map {
        {"map_Ka", "ambientSampler"}, {"map_Kd", "albedoSampler"}, {"map_Ks", "roughnessSampler"}
    };

    std::map<std::string, uint64_t> gltf_type {
        {"VEC3", sizeof(float)*3}, {"VEC2", sizeof(float)*2}, {"SCALAR", sizeof(int32_t)}
    };

    std::map<std::string, uint64_t> wavefront_object_material_map {};
protected:
    stream::serializer get_pbr_from_wavefront_object_mtllib(stream::serializer &serializer);
    uint64_t get_gltf_component_size(uint32_t n);

    bool process_wavefront_object_meshes(std::vector<stream::mesh> &meshes);
    bool process_wavefront_object_mtllib(stream::mtl &mtl);
    bool read_wavefront_object_mtllib_index(std::ifstream &ifs_wavefront_object);

    bool process_stl(stream::mesh &mesh);
    bool process_gltf(stream::mesh &mesh, nlohmann::json &gltf_node);
    bool process_gltf_meshes(std::vector<stream::mesh> &meshes);

    bool process_gltf_mtl(stream::mtl &mtl);
    bool read_gltf_mesh_bytes(stream::mesh &mesh, stream::type mesh_type, nlohmann::json &value);
public:
    stream::format get_model_format(std::string_view path);
    bool read_mesh_filename(std::string &filename, std::string_view path);
    bool read_full_filename(std::string &filename, std::string_view path);
    bool load_meshes(std::vector<stream::mesh> &meshes, std::string_view path);
    bool load_mtl(stream::mtl &mtl, std::string_view path);

    template<typename t>
    bool load_heightmap_mesh(stream::mesh &mesh, asset::texture<t> *p_texture, const glm::ivec3 &size = {0, 0, 0}) {
        glm::vec3 vertex {};
        if (p_texture != nullptr) {
            p_texture->invoke();
            p_texture->read_cpu_side();
            p_texture->revoke();

            const t divider = !std::is_same<t, float>::value ? 255 : 1.0f;
            t *&p_data {p_texture->cpu_side_data()};

            auto &gpu_side {p_texture->gpu_side_data()};
            const float fwf {size.x == 0 ? static_cast<float>(gpu_side.w) : static_cast<float>(size.x)};
            const float fhf {size.y == 0 ? static_cast<float>(gpu_side.h) : static_cast<float>(size.y)};
            const int64_t channels {gpu::channels(gpu_side)};

            for (int32_t w {}; w < static_cast<uint32_t>(fwf); w++) {
                for (int32_t h {}; h < static_cast<uint32_t>(fhf); h++) {
                    const t *p_vec {&p_data[static_cast<int64_t>(w) * static_cast<int64_t>(h) * channels]};
                    vertex.x = static_cast<float>(w) / fwf;
                    vertex.y = (static_cast<float>(p_vec[0]) + static_cast<float>(p_vec[1]) + static_cast<float>(p_vec[2])) / static_cast<float>(divider);
                    vertex.z = static_cast<float>(h) / fhf;
                    mesh.append(vertex, stream::type::vertex);
                }
            }

            glm::vec4 index {};
            for (uint32_t w {}; w < static_cast<uint32_t>(fwf) - 1; w++) {
                for (uint32_t h {}; h < static_cast<uint32_t>(fhf) - 1; h++) {
                    index.x = static_cast<float>(w * h * 3);
                    index.y = static_cast<float>((w + 1) * h * 3);
                    index.x = static_cast<float>(w * (h + 1) * 3);
                    index.w = static_cast<float>((w + 1) * (h + 1) * 3);
                    mesh.append(index, stream::type::index);
                    mesh.faces++;
                }
            }
        } else if (size.z == 0) {
            const float fwf {static_cast<float>(size.x)};
            const float fhf {static_cast<float>(size.y)};
            vertex.y = 0.0f;

            for (int32_t w {}; w < static_cast<uint32_t>(fwf); w++) {
                for (int32_t h {}; h < static_cast<uint32_t>(fhf); h++) {
                    vertex.x = static_cast<float>(w) / fwf;
                    vertex.z = static_cast<float>(h) / fhf;
                    mesh.append(vertex, stream::type::vertex);
                    mesh.append({vertex.x, vertex.z}, stream::type::texcoord);
                }
            }

            glm::vec4 index {};
            for (uint32_t w {}; w < static_cast<uint32_t>(fwf) - 1; w++) {
                for (uint32_t h {}; h < static_cast<uint32_t>(fhf) - 1; h++) {
                    index.x = static_cast<float>(w * h * 3);
                    index.y = static_cast<float>((w + 1) * h * 3);
                    index.x = static_cast<float>(w * (h + 1) * 3);
                    index.w = static_cast<float>((w + 1) * (h + 1) * 3);
                    mesh.append(index, stream::type::index);
                    mesh.faces++;
                }
            }
        } else {
            const float fwf {static_cast<float>(size.x)};
            const float fhf {static_cast<float>(size.y)};
            vertex.y = 0.0f;
            mesh.faces = size.x * size.y;

            for (int32_t w {}; w < static_cast<uint32_t>(fwf); w++) {
                for (int32_t h {}; h < static_cast<uint32_t>(fhf); h++) {
                    vertex.x = static_cast<float>(w) / fwf;
                    vertex.z = static_cast<float>(h) / fhf;
                    mesh.append(vertex, stream::type::vertex);
                    mesh.append({vertex.x, vertex.z}, stream::type::texcoord);

                    vertex.x = static_cast<float>(w + 1) / fwf;
                    vertex.z = static_cast<float>(h) / fhf;
                    mesh.append(vertex, stream::type::vertex);
                    mesh.append({vertex.x, vertex.z}, stream::type::texcoord);
                
                    vertex.x = static_cast<float>(w) / fwf;
                    vertex.z = static_cast<float>(h + 1) / fhf;
                    mesh.append(vertex, stream::type::vertex);
                    mesh.append({vertex.x, vertex.z}, stream::type::texcoord);

                    vertex.x = static_cast<float>(w + 1) / fwf;
                    vertex.z = static_cast<float>(h + 1) / fhf;
                    mesh.append(vertex, stream::type::vertex);
                    mesh.append({vertex.x, vertex.z}, stream::type::texcoord);
                }
            }
        }

        return false;
    };
};

#endif