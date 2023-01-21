#ifndef AGK_API_GPU_TOOLS_H
#define AGK_API_GPU_TOOLS_H

namespace gpu {
    typedef struct texture {
        unsigned int w {};
        unsigned int h {};
        unsigned int z {};
        unsigned int type {};
        unsigned int format {};
        unsigned int channel {};
        unsigned int id {};
        unsigned int primitive {};
    } texture;

    typedef struct frame {
        int w {};
        int h {};
        int z {};
        unsigned int type {};
        unsigned int format {};
        unsigned int id_texture {};
        unsigned int id_fbo {};
        unsigned int id_depth {};
    } frame;
}

#endif

#ifndef AGK_API_GPU_TOOLS_BUFFERING_H
#define AGK_API_GPU_TOOLS_BUFFERING_H

#include "program.hpp"
#include <glm/glm.hpp>

class buffering {
protected:
    static uint32_t current_type_bind[2];

    std::vector<uint32_t> buffer_list {};
    std::map<std::string, uint32_t> buffer_map {};

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

#ifndef AGK_API_GPU_TOOLS_PARALLELING_H
#define AGK_API_GPU_TOOLS_PARALLELING_H

class paralleling {
protected:
    std::map<std::string, uint32_t> buffer_map {};
public:
    uint32_t memory_barrier {GL_ALL_ATTRIB_BITS};
    uint32_t dispatch_groups[3] {1, 1, 1};
    uint32_t dimension[3] {};
    uint32_t map_buffer_type[2] {};

    shading::program *p_program_parallel {};

    explicit paralleling() = default;
    ~paralleling() {

    }

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
        glBindImageTexture(slot, texture.id, 0, GL_FALSE, 0, mode, texture.channel);
    }

    void invoke() const {
        glUseProgram(this->p_program_parallel->id);
    }

    void dispatch() const {
        /* Pipeline "draw call", but for target texture. */
        glDispatchCompute(this->dimension[0] / this->dispatch_groups[0], this->dimension[1] / this->dispatch_groups[1], 1 / this->dispatch_groups[2]);

        /* Do pipeline wait for process. */
        glMemoryBarrier(memory_barrier);
    }

    void revoke() const {
        glUseProgram(0);
    }
};

#endif

#ifndef AGK_API_GPU_TOOLS_FRAMEBUFFERING_H
#define AGK_API_GPU_TOOLS_FRAMEBUFFERING_H

class framebuffering {
public:
    std::map<uint32_t, gpu::frame> frame_map {};
public:
    static uint32_t current_frame_info;

    explicit framebuffering() = default;
    ~framebuffering() {}

    gpu::frame &operator[](uint32_t key) {
        return this->frame_map[key];
    }

    void send_depth(const glm::ivec3 &in_dimension, uint32_t in_format, bool attach_unique = false) {
        gpu::frame &framebuffer {this->frame_map[framebuffering::current_frame_info]};
        if (framebuffer.w != in_dimension.x && framebuffer.h == in_dimension.y && framebuffer.z == in_dimension.z) {
            return;
        }

        framebuffer.w = in_dimension.x;
        framebuffer.h = in_dimension.y;
        framebuffer.z = in_dimension.z;
        framebuffer.format = in_format;

        if (framebuffer.id_texture == 0) glGenTextures(1, &framebuffer.id_texture);
        glBindTexture(framebuffer.type, framebuffer.id_texture);

        /* Gen a storage buffer. */
        if (framebuffer.type == GL_TEXTURE_3D) {
            glTexStorage3D(framebuffer.type, 1, framebuffer.format, framebuffer.w, framebuffer.h, framebuffer.z);
        } else {
            glTexStorage2D(framebuffer.type, 1, framebuffer.format, framebuffer.w, framebuffer.h);
        }

        /* Attach renderbuffer (depth buffer) to framebuffer. */
        if (framebuffer.id_depth == 0) glGenRenderbuffers(1, &framebuffer.id_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.id_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebuffer.w, framebuffer.h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_RENDERBUFFER, framebuffer.id_depth);

        if (attach_unique) {
            GLenum enums {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, &enums);
        }
    }

    void attach(uint32_t attachment, uint32_t type, uint32_t texture) {
        if (type == GL_TEXTURE_3D) {
            glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, type, texture, 1, 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, type, texture, 0);
        }
    }

    void draw_buffers(uint32_t size, const GLenum *p_buffers) {
        glDrawBuffers(size, p_buffers);
    }

    void invoke(uint32_t key, uint32_t flags = 0) {
        /* Gen framebuffer is necessary. */
        gpu::frame &framebuffer {this->frame_map[key]};
        if (framebuffer.id_fbo == 0) glGenFramebuffers(1, &framebuffer.id_fbo);
        framebuffering::current_frame_info = key;

        /* Bind the current framebuffer and clear previous screen buffers. */
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id_fbo);
        if (flags != 0) glClear(flags);
    }

    void revoke(uint32_t flags = 0) {
        /* Unbind framebuffer and clean previous screen buffers. */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (flags != 0) glClear(flags);
        framebuffering::current_frame_info = 0;
    }

    void delete_buffer(uint32_t key) {
        gpu::frame &frame {this->frame_map[key]};
        if (frame.id_fbo != 0) glDeleteFramebuffers(1, &frame.id_fbo);
        if (frame.id_depth != 0) glDeleteRenderbuffers(1, &frame.id_depth);
        if (frame.id_texture != 0) glDeleteTextures(1, &frame.id_texture);
        this->frame_map.erase(key);
    }

    void delete_buffers() {
        for (auto const &[key, texture] : this->frame_map) {
            if (texture.id_fbo != 0) glDeleteFramebuffers(1, &texture.id_fbo);
            if (texture.id_depth != 0) glDeleteRenderbuffers(1, &texture.id_depth);
            if (texture.id_texture != 0) glDeleteTextures(1, &texture.id_texture);
        }

        this->frame_map.clear();
    }
};

#endif

#ifndef AGK_API_GPU_TOOLS_TEXTURING_H
#define AGK_API_GPU_TOOLS_TEXTURING_H

class texturing {
protected:
    std::map<uint32_t, gpu::texture> texture_map {};
public:
    static uint32_t current_texture_info[2];

    explicit texturing() = default;
    ~texturing() {};

    gpu::texture &operator[](uint32_t key) {
        return this->texture_map[key];
    }

    void invoke_active(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
    }

    void invoke(uint32_t key, const glm::ivec2 &texture_info) {
        texturing::current_texture_info[0] = key;
        texturing::current_texture_info[1] = texture_info.y;

        gpu::texture &texture {this->texture_map[key]};
        texture.type = texture_info.x;
        texture.primitive = texture_info.y;

        /* Gen texture and bind it. */
        if (texture.id == 0) glGenTextures(1, &texture.id);
        glBindTexture(texture.type, texture.id);
    }

    void revoke() {
        /* Unbind all textures. */
        glBindTexture(texturing::current_texture_info[1], 0);
        texturing::current_texture_info[0] = 0;
        texturing::current_texture_info[1] = 0;
    }

    void delete_buffers() {
       for (auto const &[key, val] : this->texture_map) {
           if (val.id != 0) glDeleteTextures(1, &val.id);
       }

       this->texture_map.clear();
    }

    void delete_buffer(uint32_t key) {
        gpu::texture &texture {this->texture_map[key]};
        if (texture.id != 0) glDeleteTextures(1, &texture.id);
        this->texture_map.erase(key);
    }

    template<typename t>
    void send(const glm::ivec3 &in_dimension, const t *p_data, const glm::ivec2 &in_format, const glm::ivec3 &in_filter = {GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE}) {
        gpu::texture &texture {this->texture_map[texturing::current_texture_info[0]]};

        texture.w = in_dimension.x;
        texture.h = in_dimension.y;
        texture.h = in_dimension.z;

        texture.format = in_format.x;
        texture.channel = in_format.y;

        /* Pass image filtering. */
        glTexParameteri(texture.type, GL_TEXTURE_MIN_FILTER, in_filter.x);
        glTexParameteri(texture.type, GL_TEXTURE_MAG_FILTER, in_filter.y);

        glTexParameteri(texture.type, GL_TEXTURE_WRAP_S, in_filter.z);
        glTexParameteri(texture.type, GL_TEXTURE_WRAP_T, in_filter.z);

        /* Send texture buffer to GPU. */
        if (texture.type == GL_TEXTURE_3D) {
            glTexParameteri(texture.type, GL_TEXTURE_WRAP_R, in_filter.z);
            glTexImage3D(texture.type, 0, texture.format, texture.w, texture.h, texture.z, 0, texture.channel, texture.primitive, p_data);
        } else {
            glTexImage2D(texture.type, 0, texture.format, texture.w, texture.h, 0, texture.channel, texture.primitive, p_data);
        }
    }

    void generate_mipmap() {
        glGenerateMipmap(texturing::current_texture_info[1]);
    }

    template<typename t>
    std::vector<t> &get(std::vector<t> &data) {
        uint32_t channel {};
        gpu::texture &texture {this->texture_map[texturing::current_texture_info[0]]};

        switch (texture.channel) {
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
        data.resize(texture.w * texture.h * channel);
        glGetTexImage(texture.type, 0, texture.channel, texture.primitive, data.data());
        return data;
    }
};

#endif