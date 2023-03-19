#ifndef AGK_STREAM_MESH_H
#define AGK_STREAM_MESH_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>

namespace stream {
    enum class format {
        unknown, wavefrontobj, stl, gltf
    };

    enum class type {
        vertex, texcoord, normal, index
    };

    struct mesh {
    protected:
        std::vector<float> v_list {};
        std::vector<float> t_list {};
        std::vector<float> n_list {};
        std::vector<uint32_t> i_list {};
    public:
        int32_t faces {};
        stream::format format {};
        std::string name {};
        typedef struct pack { std::vector<glm::vec3> v {}, n {}; std::vector<glm::vec2> t {}; } pack;
    public:
        bool contains(stream::type _type) {
            switch (_type) {
            case stream::type::vertex:
                return !this->v_list.empty();
            case stream::type::texcoord:
                return !this->t_list.empty();
            case stream::type::normal:
                return !this->n_list.empty();
            case stream::type::index:
                return !this->i_list.empty();
            }
            return false;
        }

        void append(const glm::vec4 &vec, stream::type _type) {
            switch (_type) {
                case stream::type::vertex:
                    this->v_list.push_back(vec.x);
                    this->v_list.push_back(vec.y);
                    this->v_list.push_back(vec.z);
                    this->v_list.push_back(vec.w);
                    break;
                case stream::type::texcoord:
                    this->t_list.push_back(vec.x);
                    this->t_list.push_back(vec.y);
                    this->t_list.push_back(vec.z);
                    this->t_list.push_back(vec.w);
                    break;
                case stream::type::normal:
                    this->n_list.push_back(vec.x);
                    this->n_list.push_back(vec.y);
                    this->n_list.push_back(vec.z);
                    this->n_list.push_back(vec.w);
                    break;
                case stream::type::index:
                    this->i_list.push_back(static_cast<uint32_t>(vec.x));
                    this->i_list.push_back(static_cast<uint32_t>(vec.y));
                    this->i_list.push_back(static_cast<uint32_t>(vec.z));
                    this->i_list.push_back(static_cast<uint32_t>(vec.w));
                    break;
            };
        }

        void append(const glm::vec3 &vec, stream::type _type) {
            switch (_type) {
                case stream::type::vertex:
                    this->v_list.push_back(vec.x);
                    this->v_list.push_back(vec.y);
                    this->v_list.push_back(vec.z);
                    break;
                case stream::type::texcoord:
                    this->t_list.push_back(vec.x);
                    this->t_list.push_back(vec.y);
                    this->t_list.push_back(vec.z);
                    break;
                case stream::type::normal:
                    this->n_list.push_back(vec.x);
                    this->n_list.push_back(vec.y);
                    this->n_list.push_back(vec.z);
                    break;
                case stream::type::index:
                    this->i_list.push_back(static_cast<uint32_t>(vec.x));
                    this->i_list.push_back(static_cast<uint32_t>(vec.y));
                    this->i_list.push_back(static_cast<uint32_t>(vec.z));
                    break;
            };
        }

        void append(const glm::vec2 &vec, stream::type _type) {
            switch (_type) {
                case stream::type::vertex:
                    this->v_list.push_back(vec.x);
                    this->v_list.push_back(vec.y);
                    break;
                case stream::type::texcoord:
                    this->t_list.push_back(vec.x);
                    this->t_list.push_back(vec.y);
                    break;
                case stream::type::normal:
                    this->n_list.push_back(vec.x);
                    this->n_list.push_back(vec.y);
                    break;
                case stream::type::index:
                    this->i_list.push_back(static_cast<uint32_t>(vec.x));
                    this->i_list.push_back(static_cast<uint32_t>(vec.y));
                    break;
            };
        }

        void append(float val, stream::type _type) {
            switch (_type) {
                case stream::type::vertex:
                    this->v_list.push_back(val);
                    break;
                case stream::type::texcoord:
                    this->t_list.push_back(val);
                    break;
                case stream::type::normal:
                    this->n_list.push_back(val);
                    break;
                case stream::type::index:
                    this->i_list.push_back(static_cast<uint32_t>(val));
                    break;
            };
        }

        void append(uint32_t val, stream::type _type) {
            switch (_type) {
                case stream::type::vertex:
                    this->v_list.push_back(static_cast<float>(val));
                    break;
                case stream::type::texcoord:
                    this->t_list.push_back(static_cast<float>(val));
                    break;
                case stream::type::normal:
                    this->n_list.push_back(static_cast<float>(val));
                    break;
                case stream::type::index:
                    this->i_list.push_back(val);
                    break;
            };
        }

        template<typename t>
        std::vector<t> &get(stream::type _type) {
            switch (_type) {
                case stream::type::vertex:
                    return this->v_list;
                case stream::type::texcoord:
                    return this->t_list;
                case stream::type::normal:
                    return this->n_list;
                case stream::type::index:
                    return this->i_list;
            };
        }
    };

    struct serializer {
    protected:
        std::map<std::string, std::map<std::string, std::string>> metadata {};
    public:
        inline std::map<std::string, std::string> &operator[](std::string_view k) {
            return this->metadata[k.data()];
        }

        std::map<std::string, std::map<std::string, std::string>> &get_metadata();
    };

    struct mtl {
    protected:
        stream::serializer serializer {};
    public:
        stream::format format {};
    public:
        void set_serializer(const stream::serializer &_serializer);
        stream::serializer &get_serializer();
    };
};

#endif