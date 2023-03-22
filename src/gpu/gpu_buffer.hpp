#ifndef AGK_GPU_BUFFER_H
#define AGK_GPU_BUFFER_H

#include "GL/glew.h"
#include <map>
#include <iostream>
#include <glm/glm.hpp>

class buffering {
protected:
    uint32_t current_buffer_info[2] {};
    std::map<uint32_t, uint32_t> buffer_map {};
    uint32_t buffer_vao {};
    int32_t element_index {};
public:
    int32_t primitive[2] {GL_TRIANGLES, GL_UNSIGNED_INT};
    int32_t stride[3] {0, 0, 0};

    bool instance_rendering {};
    bool indexing_rendering {};

    uint32_t &operator[](uint32_t key) {
        return this->buffer_map[key];
    }

    void tessellation(int32_t patches) {
        glPatchParameteri(GL_PATCH_VERTICES, patches);
        this->primitive[0] = GL_PATCHES;
    }

    void bind(uint32_t key, const glm::ivec2 &buffer_type) {
        auto &buffer {this->buffer_map[key]};
        if (buffer == 0) {
            glGenBuffers(1, &buffer);
        }

        switch (buffer_type.x) {
            case GL_ELEMENT_ARRAY_BUFFER: {
                this->indexing_rendering = true;
                this->primitive[1] = buffer_type.y;
                this->element_index = key;
                break;
            }
        }

        glBindBuffer(buffer_type.x, buffer);
        this->current_buffer_info[0] = buffer_type.x;
        this->current_buffer_info[1] = buffer_type.y;
    }

    void bind(const glm::ivec2 &buffer_type) {
        this->current_buffer_info[0] = buffer_type.x;
        this->current_buffer_info[1] = buffer_type.y;
    }

    template<typename t>
    void send(uint32_t size, const t *p_data, GLenum gl_driver_read_mode) {
        glBufferData(this->current_buffer_info[0], size, p_data, gl_driver_read_mode);
    }

    template<typename t>
    void edit(uint32_t offset, int64_t size, const t *p_data) {
        glBufferSubData(this->current_buffer_info[0], offset, size, p_data);
    }

    void attach(uint32_t location, int32_t vec, const glm::ivec2 attrib_stride = {0, 0}) {
        const void *p_hack {(void*) (uint64_t) attrib_stride.y};
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, vec, this->current_buffer_info[1], GL_FALSE, attrib_stride.x, p_hack);
    }

    void divisor(int32_t location, int32_t divisor) const {
        glVertexAttribDivisor(location, divisor);
    }

    void unbind() {
        glBindBuffer(this->current_buffer_info[0], 0);
        this->current_buffer_info[0] = this->current_buffer_info[1] = 0;
    }

    void invoke() {
        if (this->buffer_vao == 0) {
            glGenVertexArrays(1, &this->buffer_vao);
        }

        glBindVertexArray(this->buffer_vao);
    }

    void revoke() {
        glBindVertexArray(0);
    }

    void draw() {
        switch (this->instance_rendering) {
            case true: {
                if (this->indexing_rendering) {
                    const void *p_hack {(void*) (uint64_t) this->stride[0]};
                    glDrawElementsInstanced(this->primitive[0], this->stride[1], this->primitive[1], p_hack, this->stride[2]);
                } else {
                    glDrawArraysInstanced(this->primitive[0], this->stride[0], this->stride[1], this->stride[2]);
                }
                break;
            }

            case false: {
                if (this->indexing_rendering) {
                    const void *p_hack {(void*) (uint64_t) this->stride[0]};
                    glDrawElements(this->primitive[0], this->stride[1], this->primitive[1], p_hack);
                } else {
                    glDrawArrays(this->primitive[0], this->stride[0], this->stride[1]);
                }
                break;
            }
        }
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
        if (key == this->element_index) {
            this->indexing_rendering = false;
            this->element_index = -1;
        }

        if (buffer != 0) {
            glDeleteBuffers(1, &buffer);
        }

        this->buffer_map.erase(key);
    }
};

#endif