#include "program.hpp"
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
