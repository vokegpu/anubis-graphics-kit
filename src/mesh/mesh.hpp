#ifndef AGK_ASSET_MESH_H
#define AGK_ASSET_MESH_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <map>

namespace mesh {
    enum class format {
        unknown, wavefrontobj, stl, gltf
    };

    enum class type {
        vertex, textcoord, normal, color
    };

    struct serializer {
    protected:
        std::map<std::string, std::map<std::string, std::string>> metadata {};
    public:
        inline std::map<std::string, std::string> &operator[](std::string_view k_string) {
            return this->metadata[k_string.data()];
        };

        std::map<std::string, std::map<std::string, std::string>> &get_metadata();
    };

    struct gltf {
    };

    struct data {
    protected:
        std::vector<float> v_list {};
        std::vector<float> t_list {};
        std::vector<float> n_list {};
        std::vector<float> c_list {};

        std::vector<uint32_t> iv_list {};
        std::vector<uint32_t> it_list {};
        std::vector<uint32_t> in_list {};
        std::vector<uint32_t> ic_list {};

        uint8_t v_len {3};
        uint8_t t_len {2};
        uint8_t n_len {3};
    public:
        mesh::format format {};
        mesh::serializer pbr_serializer {};
        int32_t faces {};

        explicit inline data() = default;
        explicit inline data(mesh::format _format) : format(_format) {};
        inline ~data() = default;

        void append(mesh::type type, const glm::vec4 &vec);
        void append(mesh::type type, const glm::vec3 &vec);
        void append(mesh::type type, const glm::vec2 &vec);
        void append(mesh::type type, float value);
        void append(mesh::type type, uint32_t index);

        std::vector<uint32_t> &get_uint_list(mesh::type type);
        std::vector<float> &get_float_list(mesh::type type);
        bool contains(mesh::type type, bool indexing = false);
    };
}

#endif