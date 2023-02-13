#include "program.hpp"
#include "api/util/file.hpp"
#include "api/util/env.hpp"

void shading::program::set_uniform_vec4(std::string_view name, const float* vec) {
    glUniform4fv(this->uniform_map[name.data()], GL_TRUE, vec);
}

void shading::program::set_uniform_vec3(std::string_view name, const float* vec) {
    glUniform3fv(this->uniform_map[name.data()], GL_TRUE, vec);
}

void shading::program::set_uniform_vec2(std::string_view name, const float* vec) {
    glUniform2fv(this->uniform_map[name.data()], GL_TRUE, vec);
}

void shading::program::set_uniform_mat2(std::string_view name, const float* matrix) {
    glUniformMatrix2fv(this->uniform_map[name.data()], GL_TRUE, GL_FALSE, matrix);
}

void shading::program::set_uniform_mat3(std::string_view name, const float* matrix) {
    glUniformMatrix3fv(this->uniform_map[name.data()], GL_TRUE, GL_FALSE, matrix);
}

void shading::program::set_uniform_mat4(std::string_view name, const float* matrix) {
    glUniformMatrix4fv(this->uniform_map[name.data()], GL_TRUE, GL_FALSE, matrix);
}

void shading::program::set_uniform_float(std::string_view name, float value) {
    glUniform1f(this->uniform_map[name.data()], value);
}

void shading::program::set_uniform_int(std::string_view name, int32_t value) {
    glUniform1i(this->uniform_map[name.data()], value);
}

void shading::program::set_uniform_bool(std::string_view name, bool value) {
    glUniform1i(this->uniform_map[name.data()], (int32_t) value);
}

shading::program::program() {
    this->id = glCreateProgram();
}

shading::program::~program() {
    glDeleteProgram(this->id);
}

void shading::program::invoke() const {
    glUseProgram(this->id);
}

void shading::program::revoke() const {
    glUseProgram(0);
}

void shading::program::send() {
    int32_t active_uniforms_location {};
    glGetProgramInterfaceiv(this->id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &active_uniforms_location);

    GLenum properties[4] {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX};
    int32_t results[4] {};

    for (int32_t it {}; it < active_uniforms_location; ++it) {
        glGetProgramResourceiv(this->id, GL_UNIFORM, it, 4, properties, 4, nullptr, results);

        int32_t name_buf_size {results[0] + 1};
        char *p_data {new char[name_buf_size]};

        glGetProgramResourceName(this->id, GL_UNIFORM, it, name_buf_size, nullptr, p_data);
        this->uniform_map.insert({p_data, it});
        delete[] p_data;
    }
}
