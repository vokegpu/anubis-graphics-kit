#ifndef AGK_GPU_PROGRAMBUFFER_H
#define AGK_GPU_PROGRAMBUFFER_H

#include "GL/glew.h"
#include <iostream>
#include <glm/glm.hpp>
#include <map>

class programbuffering {
protected:
    uint32_t current_buffer_info[3] {};
    std::map<uint32_t, uint32_t> buffer_map {};
public:
    uint32_t &operator[](uint32_t k_buffer) {
        return this->buffer_map[k_buffer];
    }

    void invoke(uint32_t key, uint32_t buffer_info) {
        auto &buffer {this->buffer_map[key]};
        if (buffer == 0) {
            glGenBuffers(1, &buffer);
        }

        glBindBuffer(buffer_info, buffer);
        this->current_buffer_info[0] = buffer_info;
        this->current_buffer_info[1] = buffer;
    }

    template<typename t>
    void send(uint32_t size, const t *p_data, uint32_t gl_driver_read_mode) {
        glBufferData(this->current_buffer_info[0], size, p_data, gl_driver_read_mode);
    }

    template<typename t>
    void edit(uint32_t offset, int64_t size, const t *p_data) {
        glBufferSubData(this->current_buffer_info[0], offset, size, p_data);
    }

    void bind(uint32_t binding, const glm::ivec2 &stride = {0, 0}) {
        if (stride.x == 0 && stride.y == 0) {
            glBindBufferBase(this->current_buffer_info[0], binding, this->current_buffer_info[1]);
        } else {
            glBindBufferRange(this->current_buffer_info[0], binding, this->current_buffer_info[1], stride.x, stride.y);
        }
    }

    void attach(uint32_t program, std::string_view block, uint32_t binding) {
        uint32_t block_index {glGetUniformBlockIndex(program, block.data())};
        glUniformBlockBinding(program, block_index, binding);
    }

    void revoke() {
        glBindBuffer(this->current_buffer_info[0], 0);
    }

    void delete_buffers() {
        for (auto it {this->buffer_map.begin()}; it != this->buffer_map.end(); it = std::next(it)) {
            if (it->second != 0) glDeleteBuffers(1, &it->second);
        }

        this->buffer_map.clear();
    }

    void delete_buffer(uint32_t key) {
        if (!this->buffer_map.count(key)) {
            return;
        }

        auto &buffer {this->buffer_map[key]};
        if (buffer != 0) glDeleteBuffers(1, &buffer);
        this->buffer_map.erase(key);
    }
};

#endif