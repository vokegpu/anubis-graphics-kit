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

    explicit buffering() = default;
    ~buffering();

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
protected:
    std::vector<t> data {};
    std::map<std::string, uint32_t> buffer_map {};
public:
    uint32_t primitive {};
    uint32_t memory_barrier {GL_ALL_ATTRIB_BITS};
    uint32_t operation {GL_READ_WRITE};
    uint32_t texture_type {GL_TEXTURE_2D};
    uint32_t dispatch_groups[3] {1, 1, 1};

    uint32_t buffer_texture {};
    uint32_t dimension[3] {};
    uint32_t format[2] {};
    uint32_t map_buffer_type[2] {};

    shading::program *p_program_parallel {};

    explicit paralleling() = default;
    ~paralleling() {

    }

    template<typename v>
    void bind_map(const std::string &name, int32_t size, v *p_data, uint32_t flags, const glm::ivec2 &buffer_type) {
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

    void send(const glm::ivec3 &in_dimension, const t *p_data, const glm::ivec2 &in_format, const glm::ivec3 &in_filter = {GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE}) {
        this->dimension[0] = in_dimension.x;
        this->dimension[1] = in_dimension.y;
        this->dimension[2] = in_dimension.z;
        this->format[0] = in_format.x;
        this->format[1] = in_format.y;

        /* Pass image filtering. */
        glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, in_filter.x);
        glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, in_filter.y);

        glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, in_filter.z);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, in_filter.z);

        /* Send texture buffer to GPU. */
        if (texture_type == GL_TEXTURE_3D) {
            glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, in_filter.z);
            glTexImage3D(texture_type, 0, this->format[0], this->dimension[0], this->dimension[1], this->dimension[2], 0, this->format[1], this->primitive, p_data);
        } else {
            glTexImage2D(texture_type, 0, this->format[0], this->dimension[0], this->dimension[1], 0, this->format[1], this->primitive, p_data);
        }
    }

    void bind_texture() {
        glBindTexture(this->texture_type, this->buffer_texture);
    }

    void attach(uint32_t slot, const glm::ivec2 &image_type = {0, GL_RGBA32F}) {
        uint32_t texture = image_type.x;
        if (texture == 0) texture = this->buffer_texture;

        /* Bind image to the compute shader. */
        glBindImageTexture(slot, texture, 0, GL_FALSE, 0, this->operation, image_type.y);
    }

    void invoke() {
        if (this->buffer_texture == 0) {
            glGenTextures(1, &this->buffer_texture);
        }

        glUseProgram(this->p_program_parallel->id);
        glBindTexture(this->texture_type, this->buffer_texture);
    }

    void invoke_texture() {
        glBindTexture(this->texture_type, this->buffer_texture);
    }

    void revoke_texture() {
        glBindTexture(this->texture_type, 0);
    }

    void dispatch() {
        /* Pipeline "draw call", but for target texture. */
        glDispatchCompute(this->dimension[0] / this->dispatch_groups[0], this->dimension[1] / this->dispatch_groups[1], 1 / this->dispatch_groups[2]);

        /* Do pipeline wait for process. */
        glMemoryBarrier(memory_barrier);
    }

    void overwrite() {
        if (texture_type == GL_TEXTURE_3D) {
            glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, this->filter[3]);
            glTexImage3D(texture_type, 0, this->format[0], this->dimension[0], this->dimension[1], this->dimension[2], 0, this->format[1], this->primitive, this->data.data());
        } else {
            glTexImage2D(texture_type, 0, this->format[0], this->dimension[0], this->dimension[1], 0, this->format[1], this->primitive, this->data.data());
        }
    }

    void revoke() {
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    void free_buffers() {
        if (this->buffer_texture != 0) glDeleteTextures(1, &this->buffer_texture);
        this->buffer_texture = 0;
    }

    std::vector<t> &get() {
        int32_t channel {1};
        switch (this->format[1]) {
            case GL_RG: {
                channel = 2;
                break;
            }

            case GL_RGB: {
                channel = 3;
                break;
            }


            case GL_RGBA: {
                channel = 4;
                break;
            }
        }

        /* Get the new texture data from GPU. */
        this->data.clear();
        this->data.resize(this->dimension[0] * this->dimension[1] * channel);
        glGetTexImage(this->texture_type, 0, this->format[1], this->primitive, this->data.data());
        return this->data;
    }

    uint32_t &get_texture() {
        return this->buffer_texture;
    }
};

#endif

#ifndef AGK_API_GPU_BUFFERING_FRAMEBUFFERING_H
#define AGK_API_GPU_BUFFERING_FRAMEBUFFERING_H

class framebuffering {
protected:
    int32_t w {};
    int32_t h {};

    uint32_t buffer_texture {};
    uint32_t buffer_fbo {};
    uint32_t buffer_renderbuffer {};
public:
    uint32_t get_texture();
    uint32_t get_fbo();
    uint32_t get_depth_buffer();

    explicit framebuffering() = default;
    ~framebuffering();

    void send(int32_t width, int32_t height, const glm::ivec2 &format = {GL_RGBA32F, GL_RGBA});
    void invoke();
    void revoke();
    void free_buffers();
};

#endif

#ifndef AGK_API_GPU_BUFFERING_TEXTURING_H
#define AGK_API_GPU_BUFFERING_TEXTURING_H

template<typename t>
class texturing {
protected:
    uint32_t buffer_texture {};
    std::vector<t> data_list {};
public:
    uint32_t primitive {GL_UNSIGNED_BYTE};
    uint32_t texture_type {GL_TEXTURE_2D};
    uint32_t dimension[3] {};
    uint32_t format[2] {};

    explicit texturing() = default;
    ~texturing() {};

    uint32_t get_texture() {
        return this->buffer_texture;
    }

    void free_buffers() {
        if (this->buffer_texture != 0) glDeleteTextures(1, &this->buffer_texture);
    }

    void invoke() {
        if (this->buffer_texture == 0) glGenTextures(1, &this->buffer_texture);
        glBindTexture(this->texture_type, this->buffer_texture);
    }

    void revoke() {
        glBindTexture(this->texture_type, 0);
    }

    void send(const glm::ivec3 &in_dimension, const t *p_data, const glm::ivec2 &in_format, const glm::ivec3 &in_filter = {GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE}) {
        this->dimension[0] = in_dimension.x;
        this->dimension[1] = in_dimension.y;
        this->dimension[2] = in_dimension.z;
        this->format[0] = in_format.x;
        this->format[1] = in_format.y;

        /* Pass image filtering. */
        glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, in_filter.x);
        glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, in_filter.y);

        glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, in_filter.z);
        glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, in_filter.z);

        /* Send texture buffer to GPU. */
        if (texture_type == GL_TEXTURE_3D) {
            glTexParameteri(texture_type, GL_TEXTURE_WRAP_R, in_filter.z);
            glTexImage3D(texture_type, 0, this->format[0], this->dimension[0], this->dimension[1], this->dimension[2], 0, this->format[1], this->primitive, p_data);
        } else {
            glTexImage2D(texture_type, 0, this->format[0], this->dimension[0], this->dimension[1], 0, this->format[1], this->primitive, p_data);
        }
    }

    std::vector<t> &data() {
        int32_t channel {1};
        switch (this->format[1]) {
            case GL_RG: {
                channel = 2;
                break;
            }

            case GL_RGB: {
                channel = 3;
                break;
            }


            case GL_RGBA: {
                channel = 4;
                break;
            }
        }

        /* Get the new texture data from GPU. */
        this->data_list.clear();
        this->data_list.resize(this->dimension[0] * this->dimension[1] * channel);
        glGetTexImage(this->texture_type, 0, this->format[1], this->primitive, this->data_list.data());
        return this->data_list;
    }
};

#endif