#include "shader.hpp"

void asset::shader::invoke() {
    glUseProgram(this->program);
}

void asset::shader::revoke() {
    glUseProgram(0);
}

int32_t &asset::shader::get_program() {
    return this->program;
}

void asset::shader::set_uniform_vec4(std::string_view name, const float* vec) {
    glUniform4fv(this->uniform_unordered_map[name.data()], GL_TRUE, vec);
}

void asset::shader::set_uniform_vec3(std::string_view name, const float* vec) {
    glUniform3fv(this->uniform_unordered_map[name.data()], GL_TRUE, vec);
}

void asset::shader::set_uniform_vec2(std::string_view name, const float* vec) {
    glUniform2fv(this->uniform_unordered_map[name.data()], GL_TRUE, vec);
}

void asset::shader::set_uniform_mat2(std::string_view name, const float* matrix) {
    glUniformMatrix2fv(this->uniform_unordered_map[name.data()], GL_TRUE, GL_FALSE, matrix);
}

void asset::shader::set_uniform_mat3(std::string_view name, const float* matrix) {
    glUniformMatrix3fv(this->uniform_unordered_map[name.data()], GL_TRUE, GL_FALSE, matrix);
}

void asset::shader::set_uniform_mat4(std::string_view name, const float* matrix) {
    glUniformMatrix4fv(this->uniform_unordered_map[name.data()], GL_TRUE, GL_FALSE, matrix);
}

void asset::shader::set_uniform_float(std::string_view name, float value) {
    glUniform1f(this->uniform_unordered_map[name.data()], value);
}

void asset::shader::set_uniform_int(std::string_view name, int32_t value) {
    glUniform1i(this->uniform_unordered_map[name.data()], value);
}

void asset::shader::set_uniform_bool(std::string_view name, bool value) {
    glUniform1i(this->uniform_unordered_map[name.data()], (int32_t) value);
}

void asset::shader::on_create() {
    feature::on_create();

    int32_t active_uniforms_location {};
    glGetProgramInterfaceiv(this->program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &active_uniforms_location);

    GLenum properties[4] {GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX};
    int32_t results[4] {};

    for (int32_t it {}; it < active_uniforms_location; ++it) {
        glGetProgramResourceiv(this->program, GL_UNIFORM, it, 4, properties, 4, nullptr, results);

        int32_t name_buf_size {results[0] + 1};
        char *p_data {new char[name_buf_size]};

        glGetProgramResourceName(this->program, GL_UNIFORM, it, name_buf_size, nullptr, p_data);
        this->uniform_unordered_map.insert({p_data, it});
        delete[] p_data;
    }
}
