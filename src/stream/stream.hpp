#ifndef AGK_STREAM_MESH_H
#define AGK_STREAM_MESH_H

#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>

namespace stream {
    enum format {
        unknown, wavefrontobj, stl, gltf
    };

    enum type {
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
        typedef struct pack { glm::vec3 v {}, t {}, n {}; } pack;
    public:
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
                    this->t_list.push_back(vec.x);
                    this->t_list.push_back(vec.y);
                    this->t_list.push_back(vec.z);
                    this->t_list.push_back(vec.w);
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
                    this->t_list.push_back(vec.x);
                    this->t_list.push_back(vec.y);
                    this->t_list.push_back(vec.z);
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
                    this->t_list.push_back(vec.x);
                    this->t_list.push_back(vec.y);
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
                case stream::type::textcoord:
                    this->t_list.push_back(val);
                    break;
                case stream::type::normal:
                    this->n_list.push_back(val);
                    break;
                default:
                    break;
            };
        }

        void append(uint32_t val, stream::type _type) {
            switch (_type) {
                case stream::type::vertex:
                    this->i_list.push_back(val);
                    break;
                default:
                    break;
            };
        }

        template<typename t>
        std::vector<t> &get(stream::type _type) {
            switch (_type) {
                case stream::type::vertex:
                    return this->v_list;
                case stream::type::textcoord:
                    return this->t_list;
                case stream::type::normal:
                    return this->n_list;
            };
            
            return this->i_list;
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
        stream::format fomrat {};
    public:
        inline explicit &operator stream::serializer() {
            return this->serializer;
        }

        void stream::serializer &set_serialiazer(const stream::serializer &_serialiazer);
        stream::serializer &get_serializer();
    };
};

#endif