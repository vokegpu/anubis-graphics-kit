#include "buffering.hpp"
#include "api/util/env.hpp"

void buffering::bind() {
    GLuint vbo {};

    if (this->buffer_list.size() >= this->buffer_list_size) {
        glGenBuffers(1, &vbo);
        this->buffer_list.push_back(vbo);
    } else {
        vbo = this->buffer_list.at(this->buffer_list_size);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    this->buffer_list_size++;
}

void buffering::send_data(GLint size, void* data, GLuint draw_mode) {
    glBufferData(GL_ARRAY_BUFFER, size, data, draw_mode);
}

void buffering::shader(GLuint location, GLint vec_rows, GLint begin, GLsizeiptr end) {
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, vec_rows, GL_FLOAT, GL_FALSE, begin, (void*) end);
}

void buffering::shader_instanced(GLuint location, GLint vec_rows, GLsizeiptr vec_columns, GLsizeiptr size) {
    GLsizeiptr stride_size {};

    for (GLuint shader_location {location}; shader_location < location + vec_columns; shader_location++) {
        glEnableVertexAttribArray(shader_location);
        glVertexAttribPointer(shader_location, vec_rows, GL_FLOAT, GL_FALSE, size, (void*) stride_size);
        glVertexAttribDivisor(shader_location, 1);
        stride_size += vec_rows;
    }
}

void buffering::bind_ebo() {
    if (this->buffer_ebo == 0) {
        glGenBuffers(1, &this->buffer_ebo);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer_ebo);
}

void buffering::send_indexing_data(GLint size, void* data, GLuint draw_mode) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, draw_mode);
}

void buffering::invoke() {
    this->buffer_list_size = 0;
    if (this->buffer_vao == 0) glGenVertexArrays(1, &this->buffer_vao);
    glBindVertexArray(this->buffer_vao);
}

void buffering::revoke() {
    if (this->buffer_list_size < this->buffer_list.size()) {
        const std::vector<GLuint> cache {this->buffer_list};
        for (size_t it {this->buffer_list_size}; it < cache.size(); it++) {
            glDeleteBuffers(1, &cache.at(it));
            this->buffer_list.erase(this->buffer_list.begin() + it);
        }
    }

    glBindVertexArray(0);
}

void buffering::destroy() {
    feature::on_destroy();

    if (this->buffer_vao != 0) {
        glDeleteVertexArrays(1, &this->buffer_vao);
    }

    if (this->buffer_ebo != 0) {
        glDeleteBuffers(1, &this->buffer_ebo);
    }

    for (uint32_t &buffers : this->buffer_list) {
        glDeleteBuffers(1, &buffers);
    }

    this->buffer_list.clear();
}


void buffering::draw() {
    glBindVertexArray(this->buffer_vao);

    if (this->buffer_ebo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer_ebo);
    }

    switch (this->mode) {
        case bufferingmode::normal: {
            if (this->buffer_ebo == 0) {
                glDrawArrays(this->primitive, 0, this->stride_end);
            } else {
                glDrawElements(this->primitive, this->stride_end, GL_UNSIGNED_INT, 0);
            }

            break;
        }

        case bufferingmode::instanced: {
            if (this->buffer_ebo == 0) {
                glDrawArraysInstanced(this->primitive, 0, this->stride_end, this->instanced_size);
            } else {
                glDrawElementsInstanced(this->primitive, this->stride_end, GL_UNSIGNED_INT, (void*) 0, this->instanced_size);
            }

            break;
        }
    }

    glBindVertexArray(0);
}