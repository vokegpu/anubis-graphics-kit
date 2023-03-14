#ifndef AGK_STREAM_PARSER_H
#define AGK_STREAM_PARSER_H

#include "stream.hpp"
#include "asset/texture.hpp"

class streamparser {
protected:
    stream::serializer current_serialiazer {};
    std::string current_path {};

    std::map<std::string, mesh::format> mesh_ext_map {
            {"stl", mesh::format::stl}, {"obj", mesh::format::wavefrontobj}, {"gltf", mesh::format::gltf}
    };

    std::map<std::string, std::string> mtl_pbr_conversions_map {
        {"map_Ka", "ambientMap"}, {"map_Kd", "albedoMap"}, {"map_Ks", "specularMap"}
    };
protected:
    stream::format get_model_format(std::string_view path);
    stream::serializer get_pbr_from_wavefront_object_mtllib(stream::serializer &serializer);

    bool process_wavefront_object(stream::mesh &mesh);
    bool process_wavefront_object_mtllib(stream::mtl &mtl);
    bool process_stl(stream::mesh &mesh);
    bool process_gltf(stream::mesh &mesh);
    bool process_gltf_mtl(stream::mtl &mtl);
public:
    bool load_mesh(stream::mesh &mesh, std::string_view path);
    bool load_mtl(stream::mtl &mtl, std::string_view path);

    template<typename t>
    bool load_heightmap_mesh(stream::mesh &mesh, asset::texture<t> *p_texture, const glm::ivec2 &size = {0, 0}) {
        glm::vec3 vertex {};
        if (p_texture != nullptr) {
            p_texture->invoke();
            p_texture->read_cpu_side();
            p_texture->revoke();

            const t divider = !std::is_same<t, float>::value ? 255 : 1.0f;
            t *&p_data {p_texture->get_cpu_side()};

            auto &gpu_side {p_texture->get_gpu_side()};
            const float fwf {size.x == 0 ? static_cast<float>(gpu_side.w) : static_cast<float>(size.x)};
            const float fhf {size.y == 0 ? static_cast<float>(gpu_side.h) : static_cast<float>(size.y)};
            const int64_t channels {gpu::channels(gpu_side)};

            for (float w {}; w < fwf; w++) {
                for (float h {}; h < fhf; h++) {
                    const t *p_vec {&p_data[static_cast<int64_t>(w) * static_cast<int64_t>(h) * channels]};
                    vertex.x = w / fwf;
                    vertex.y = (static_cast<float>(p_vec[0]) + static_cast<float>(p_vec[1]) + static_cast<float>(p_vec[2])) / static_cast<float>(divider);
                    vertex.z = h / fhf;
                    mesh.append(vertex, stream::type::vertex);
                }
            }
        } else {
            const float fwf {static_cast<float>(size.x)};
            const float fhf {static_cast<float>(size.y)};

            for (float w {}; w < fwf - 1.0f; w++) {
                for (float h {}; h < fhf - 1.0f; h++) {
                    vertex.x = w / fwf;
                    vertex.y = 0.0f;
                    vertex.z = h / fhf;
                    mesh.append(vertex, stream::type::vertex);
                }
            }
        }

        glm::vec4 index {};
        for (uint32_t w {}; w < static_cast<uint32_t>(fwf) - 1; w++) {
            for (uint32_t h {}; h < static_cast<uint32_t>(fhf) - 1; h++) {
                index.x = w * h * 3;
                index.y = (w + 1) * h * 3;
                index.x = w * (h + 1) * 3;
                index.w = (w + 1) * (h + 1) * 3;
                mesh.append(index, stream::typpe::index);
                mesh.faces++;
            }
        }
    }
};

#endif