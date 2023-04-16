#ifndef AGK_STREAM_MESH_H
#define AGK_STREAM_MESH_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>

namespace stream {
    std::string f(float value);
    std::string i(int32_t value);
    std::string vec(float x, float y, float z, float w);
    std::string vec(float x, float y, float z);
    std::string vec(float x, float y);

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
        std::string tag {};
        uint32_t material {};
        typedef struct pack { std::vector<glm::vec3> v {}, n {}; std::vector<glm::vec2> t {}; } pack;
    public:
        void reserve(uint64_t size, stream::type _type) {
            switch (_type) {
            case stream::type::vertex:
                this->v_list.reserve(size);
                break;
            case stream::type::texcoord:
                this->t_list.reserve(size);
                break;
            case stream::type::normal:
                this->n_list.reserve(size);
                break;
            case stream::type::index:
                this->i_list.reserve(size);
                break;
            }
        }

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

        std::vector<uint32_t> &get_uint_list(stream::type _type) {
            return this->i_list;
        }

        std::vector<float> &get_float_list(stream::type _type) {
            switch (_type) {
                case stream::type::vertex:
                    return this->v_list;
                case stream::type::texcoord:
                    return this->t_list;
                default:
                    break;
            }

            return this->n_list;
        }

        inline void operator+=(stream::mesh &mesh) {
            this->faces += mesh.faces;

            if (mesh.contains(stream::type::vertex)) {
                auto &list {mesh.get_float_list(stream::type::vertex)};
                this->v_list.insert(this->v_list.end(), list.begin(), list.end());
            }

            if (mesh.contains(stream::type::texcoord)) {
                auto &list {mesh.get_float_list(stream::type::texcoord)};
                this->t_list.insert(this->t_list.end(), list.begin(), list.end());
            }

            if (mesh.contains(stream::type::normal)) {
                auto &list {mesh.get_float_list(stream::type::normal)};
                this->n_list.insert(this->n_list.end(), list.begin(), list.end());
            }

            if (mesh.contains(stream::type::index)) {
                auto &list {mesh.get_uint_list(stream::type::index)};
                this->i_list.insert(this->i_list.end(), list.begin(), list.end());
            }
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