#ifndef AGK_GPU_PARALLEL_H
#define AGK_GPU_PARALLEL_H

#include "asset/shader.hpp"
#include <unordered_map>
#include <iostream>
#include <glm/glm.hpp>

class paralleling {
protected:
    std::unordered_map<std::string, uint32_t> buffer_map {};
public:
    uint32_t memory_barrier {GL_ALL_BARRIER_BITS};
    uint32_t dispatch_groups[3] {1, 1, 1};
    uint32_t dimension[3] {1, 1, 1};
    uint32_t map_buffer_type[2] {};

    ::asset::shader *p_program {};

    explicit paralleling() = default;
    ~paralleling() = default;

    template<typename v>
    void send_bind(const std::string &name, int32_t size, v *p_data, uint32_t flags, const glm::ivec2 &buffer_type) {
        this->map_buffer_type[0] = buffer_type.x;
        this->map_buffer_type[1] = buffer_type.y;

        if (this->buffer_map[name] == 0) glGenBuffers(1, &this->buffer_map[name]);
        glBindBuffer(buffer_type.x, this->buffer_map[name]);
        glBufferStorage(buffer_type.x, sizeof(v), p_data, flags);
    }

    template<typename v>
    void invoke_bind(const std::string &name, v *&p_data, int32_t size = 1) {
        p_data = (v*) glMapBufferRange(this->map_buffer_type[0], 0, sizeof(v) * size, this->map_buffer_type[1]);
    }

    void revoke_bind() {
        glUnmapBuffer(this->map_buffer_type[0]);
    }

    void attach(uint32_t slot, gpu::texture &texture, uint32_t mode) const {
        /* Bind image to the compute shader. */
        glBindImageTexture(slot, texture.id, 0, GL_FALSE, 0, mode, texture.format);
    }

    void invoke() const {
        this->p_program->invoke();
    }

    void dispatch() const {
        /* Pipeline "draw call", but for target texture. */
        glDispatchCompute(this->dimension[0] / this->dispatch_groups[0], this->dimension[1] / this->dispatch_groups[1], this->dimension[2] / this->dispatch_groups[2]);

        /* Do pipeline wait for process. */
        glMemoryBarrier(memory_barrier);
    }

    void revoke() const {
        glUseProgram(0);
    }
};

#endif