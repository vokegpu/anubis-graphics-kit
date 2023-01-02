#include "buffering.hpp"
#include "api/util/env.hpp"

uint32_t buffering::current_type_bind[2] {};

void buffering::bind(const glm::ivec2 &buffer_type) {
    uint32_t buffer {};
    if (buffer_type.x == GL_ELEMENT_ARRAY_BUFFER) {
        if (this->buffer_ebo != 0) glGenBuffers(1, &this->buffer_ebo);
        buffer = this->buffer_ebo;
    }

    if (buffer == 0) {
        if (this->buffer_list_size == this->buffer_list.size()) glGenBuffers(1, &this->buffer_list.emplace_back());
        buffer = this->buffer_list.at(this->buffer_list_size++);
    }

    glBindBuffer(buffer_type.x, buffer);
    buffering::current_type_bind[0] = buffer_type.x;
    buffering::current_type_bind[1] = buffer_type.y;
}

void buffering::send(size_t size, void *p_data, uint32_t gl_driver_read_mode) {
    glBufferData(buffering::current_type_bind[0], size, p_data, gl_driver_read_mode);
}

void buffering::attach(uint32_t location, uint32_t vec, const glm::ivec2 &stride) {
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, vec, buffering::current_type_bind[1], GL_FALSE, stride.x, (void*) (stride.y));
}

void buffering::invoke() {
    if (this->buffer_vao == 0) {
        glGenVertexArrays(1, &this->buffer_vao);
    }

    glBindVertexArray(this->buffer_vao);
}

void buffering::revoke() {
    glBindVertexArray(0);
}

void buffering::draw() {
    glBindVertexArray(this->buffer_vao);

    switch (this->type) {
        case buffering::type::direct: {
            if (this->buffer_ebo != 0) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer_ebo);
                glDrawElements(this->primitive, stride[1], GL_UNSIGNED_INT, (void*) 0);
            } else {
                glDrawArrays(this->primitive, stride[0], stride[1]);
            }
        }

        case buffering::type::instanced: {
            if (this->buffer_ebo != 0) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffer_ebo);
                glDrawElementsInstanced(this->primitive, stride[1], GL_UNSIGNED_INT, (void*) 0, stride[2]);
            } else {
                glDrawArraysInstanced(this->primitive, stride[0], stride[1], stride[2]);
            }
            break;
        }
    }

    glBindVertexArray(0);
}