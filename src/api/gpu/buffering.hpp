#ifndef AGK_API_GPU_BUFFERING_H
#define AGK_API_GPU_BUFFERING_H

#include "shading.hpp"
#include <glm/glm.hpp>

class buffering {
protected:
    static uint32_t current_type_bind[2];

    std::vector<uint32_t> buffer_list {};
    uint32_t buffer_vao {}, buffer_ebo {};
    uint32_t buffer_list_size {};
public:
    enum class type {
        direct, instanced    
    };

    buffering::type type {};
    uint32_t primitive {GL_TRIANGLES};
    uint64_t stride[3] {};

    void tessellation(int32_t patches);
    void bind(const glm::ivec2 &buffer_type);
    void send(size_t size, void *p_data, uint32_t gl_driver_read_mode);
    void attach(int32_t location, int32_t vec, const glm::ivec2 &array_stride = {0, 0});
    
    void invoke();
    void revoke();
    void draw();
    void free_buffers();
};

#endif

#ifndef AGK_API_GPU_BUFFERING_PARALLELING_H
#define AGK_API_GPU_BUFFERING_PARALLELING_H

template<typename t>
class paralleling {
public:
    std::vector<t> data {};

    uint32_t texture {};
    uint32_t dimension[2] {};
    uint32_t format[2] {};
public:
    uint32_t primitive {};
    shading::program *p_program_parallel {};

    void send(const glm::ivec2 &in_dimension, void *p_data, const glm::ivec2 &in_format, const glm::ivec2 &in_filter = {GL_LINEAR, GL_LINEAR}) {
        this->dimension[0] = in_dimension.x;
        this->dimension[1] = in_dimension.y;
        this->format[0] = in_format.x;
        this->format[1] = in_format.y;

        /* Pass image filtering. */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, in_filter.x);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, in_filter.y);

        /* Send texture buffer to GPU. */
        glTexImage2D(GL_TEXTURE_2D, 0, this->format[0], this->dimension[0], this->dimension[1], 0, this->format[1], this->primitive, p_data);
    }

    void attach() {
        /* Bind image to the compute shader. */
        glBindImageTexture(0, this->texture, 0, GL_FALSE, 0, GL_READ_WRITE, this->format[0]);
    }

    void invoke() {
        if (this->p_program_parallel == nullptr) {
            return;
        }

        if (this->texture == 0) {
            glGenTextures(1, &this->texture);
        }

        glUseProgram(this->p_program_parallel->id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->texture);
    }

    void dispatch() {
        /* Pipeline "draw call", but for target texture. */
        glDispatchCompute(this->dimension[0], this->dimension[1], 1);
        
        /* Do pipeline wait for process. */
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }

    void revoke() {
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    void free_buffers() {
        if (this->texture != 0) glDeleteTextures(1, &this->texture);
        this->texture = 0;
    }

    std::vector<t> &get_data() {
        /* Get the new texture data from GPU. */
        this->data.resize(this->dimension[0] * this->dimension[1]);
        glGetTexImage(GL_TEXTURE_2D, 0, this->format[1], this->primitive, this->data.data());
        return this->data;
    }
};

#endif