#ifndef AGK_ASSET_SHADER_H
#define AGK_ASSET_SHADER_H

#include <iostream>
#include <unordered_map>
#include "GL/glew.h"
#include "util/file.hpp"
#include "util/env.hpp"
#include "core/imodule.hpp"
#include <vector>

namespace asset {
    typedef struct shader_resource {
        std::string in {};
        uint32_t stage {};
        bool is_src_the_in {};
    } shader_resource;

    class shader : public imodule {
    protected:
        std::unordered_map<std::string, int32_t> uniform_unordered_map {};
        int32_t program {};
    public:
        explicit shader(std::string_view shader_tag, const std::vector<asset::shader_resource> &resource_list) : program(glCreateProgram()) {
            this->tag = shader_tag;

            bool flag {!resource_list.empty()};
            uint32_t shader {};
            const char *p_shader_resource {};
            std::string shader_source {};
            std::vector<uint32_t> compiled_shader_list {};

            for (const asset::shader_resource &resource : resource_list) {
                shader_source.clear();

                if (resource.is_src_the_in) {
                    shader_source = resource.in;
                } else {
                    flag = util::read_file(resource.in.data(), shader_source);
                }

                if (shader_source.empty()) {
                    util::log("Invalid shader resource code");
                    util::log(resource.in);
                    break;
                }

                p_shader_resource = shader_source.data();

                shader = glCreateShader(resource.stage);
                glShaderSource(shader, 1, &p_shader_resource, nullptr);
                glCompileShader(shader);

                GLint status {};
                glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

                if (!status) {
                    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &status);
                    std::string msg {}; msg.resize(status);
                    glGetShaderInfoLog(shader, status, nullptr, msg.data());
                    util::log("Failed to compiled this shader");
                    util::log(msg);

                    this->is_dead = true;
                    break;
                }

                if (!flag) {
                    util::log(tag);
                    this->is_dead = true;
                    break;
                }

                glAttachShader(this->program, shader);
                compiled_shader_list.push_back(shader);
            }

            if (flag) {
                GLint status {};
                glLinkProgram(this->program);
                glGetProgramiv(this->program, GL_LINK_STATUS, &status);

                if (!status) {
                    glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &status);
                    std::string msg {}; msg.resize(status);
                    glGetProgramInfoLog(this->program, status, nullptr, msg.data());
                    util::log("Failed to link this shading program!");
                    util::log(msg);
                    this->is_dead = true;
                }
            }

            for (uint32_t &compiled_shaders : compiled_shader_list) {
                glDeleteShader(compiled_shaders);
            }
        };

        ~shader() {
            glDeleteProgram(this->program);
        };

        void invoke();
        void revoke();

        void set_uniform_vec4(std::string_view uniform_name, const float *p_value);
        void set_uniform_vec3(std::string_view uniform_name, const float *p_value);
        void set_uniform_vec2(std::string_view uniform_name, const float *p_value);
        void set_uniform_mat2(std::string_view uniform_name, const float *p_value);
        void set_uniform_mat3(std::string_view uniform_name, const float *p_value);
        void set_uniform_mat4(std::string_view uniform_name, const float *p_value);

        void set_uniform_float(std::string_view uniform_name, float value);
        void set_uniform_int(std::string_view uniform_name, int32_t value);
        void set_uniform_bool(std::string_view uniform_name, bool value);

        int32_t &get_program();
        void on_create() override;
    };
}

#endif