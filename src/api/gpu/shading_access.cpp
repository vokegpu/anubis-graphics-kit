#include "shading_access.hpp"
#include "api/util/env.hpp"
#include "api/util/file.hpp"

void shading::program::set_uniform_vec4(std::string_view name, const float* vec) {
    glUniform4fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, vec);
}

void shading::program::set_uniform_vec3(std::string_view name, const float* vec) {
    glUniform3fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, vec);
}

void shading::program::set_uniform_vec2(std::string_view name, const float* vec) {
    glUniform2fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, vec);
}

void shading::program::set_uniform_mat2(std::string_view name, const float* matrix) {
    glUniformMatrix2fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, GL_FALSE, matrix);
}

void shading::program::set_uniform_mat3(std::string_view name, const float* matrix) {
    glUniformMatrix3fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, GL_FALSE, matrix);
}

void shading::program::set_uniform_mat4(std::string_view name, const float* matrix) {
    glUniformMatrix4fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, GL_FALSE, matrix);
}

void shading::program::set_uniform_float(std::string_view name, float value) {
    glUniform1f(glGetUniformLocation(this->id, name.data()), value);
}

void shading::program::set_uniform_int(std::string_view name, int32_t value) {
    glUniform1i(glGetUniformLocation(this->id, name.data()), value);
}

void shading::program::set_uniform_bool(std::string_view name, bool value) {
    glUniform1i(glGetUniformLocation(this->id, name.data()), (int32_t) value);
}

bool shading_manager::create_shading_program(std::string_view shading_program_name, shading::program &program, const std::vector<shading::resource> &resources) {
    bool flag {!resources.empty()};
    std::string shader_src {};
    program.id = glCreateProgram();

    for (shading::resource resource : resources) {
        shader_src.clear();
        flag = flag && util::readfile(resource.path.data(), shader_src);
        flag = flag && this->compile_shader(resource.shader_id, resource.stage, shader_src.data());

        if (!flag) {
            break;
        }

        glAttachShader(program.id, resource.shader_id);
    }

    if (flag) {
        GLint status {};
        glLinkProgram(program.id);
        glGetProgramiv(program.id, GL_LINK_STATUS, &status);

        if (!status) {
            char log[256];
            glGetProgramInfoLog(program.id, 256, nullptr, log);
            util::log("failed to link shading program");
            util::log(log);
        } else {
            this->shading_programs_map[shading_program_name] = program.id;
            util::log(std::string(shading_program_name) + " shading program successfully linked");
        }

        for (shading::resource resource : resources) {
            glDeleteShader(resource.shader_id);
        }

        return status;
    }

    return false;
}

bool shading_manager::compile_shader(GLuint &shader, GLuint mode, const char* resource) {
    shader = glCreateShader(mode);
    glShaderSource(shader, 1, &resource, nullptr);
    glCompileShader(shader);

    GLint status {};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (!status) {
        char log[256];
        glGetShaderInfoLog(shader, 256, nullptr, log);
        util::log("failed to compile shader");
        util::log(log);
        return false;
    }

    return true;
}

void shading_manager::quit() {
    for (auto &entry : this->shading_programs_map) {
        auto &shader_program_id {entry.second};
        glDeleteProgram(shader_program_id);
    }
}
