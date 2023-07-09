#ifndef AGK_GPU_FRAMEBUFFER_H
#define AGK_GPU_FRAMEBUFFER_H

#include <unordered_map>
#include <glm/glm.hpp>
#include "gpu.hpp"

class framebuffering {
protected:
    std::unordered_map<uint32_t, gpu::frame> frame_map {};
    uint32_t current_frame_info {};
public:
    explicit framebuffering() = default;
    ~framebuffering() = default;

    gpu::frame &operator[](uint32_t key) {
        return this->frame_map[key];
    }

    void send_depth(const glm::ivec3 &in_dimension, const glm::ivec2 &in_format, bool attach_unique = false) {
        gpu::frame &framebuffer {this->frame_map[this->current_frame_info]};
        if (framebuffer.w == in_dimension.x && framebuffer.h == in_dimension.y && framebuffer.z == in_dimension.z) {
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (framebuffer.id_fbo != 0) {
            glDeleteFramebuffers(1, &framebuffer.id_fbo);
        }

        glGenFramebuffers(1, &framebuffer.id_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id_fbo);

        framebuffer.w = in_dimension.x;
        framebuffer.h = in_dimension.y;
        framebuffer.z = in_dimension.z;
        framebuffer.format = in_format.y;
        framebuffer.type = in_format.x;

        glDeleteTextures(1, &framebuffer.id_texture);
        glGenTextures(1, &framebuffer.id_texture);
        glBindTexture(framebuffer.type, framebuffer.id_texture);

        glTexParameteri(framebuffer.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(framebuffer.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        /* Gen a storage buffer. */
        if (framebuffer.type == GL_TEXTURE_3D) {
            glTexStorage3D(framebuffer.type, 1, framebuffer.format, framebuffer.w, framebuffer.h, framebuffer.z);
        } else {
            glTexStorage2D(framebuffer.type, 1, framebuffer.format, framebuffer.w, framebuffer.h);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, framebuffer.type, framebuffer.id_texture, 0);

        /* Depth sampler texture. */
        if (framebuffer.id_depth == 0) glGenTextures(1, &framebuffer.id_depth);
        glBindTexture(GL_TEXTURE_2D, framebuffer.id_depth);

        // Set filter linear.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Apply clamp to border and the border color.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Interpolate the depth result to generate only value.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, framebuffer.w, framebuffer.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer.id_depth, 0);

        /* Allocate draw buffers to framebuffer. */
        if (attach_unique) {
            GLenum enums[] {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, enums);
        }

        GLenum enums = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        std::string msg {"Framebuffer key "};
        msg += std::to_string(framebuffering::current_frame_info);

        if (enums == GL_FRAMEBUFFER_COMPLETE) {
            msg += " complete";
        } else {
            msg += " incomplete";
        }

        util::log(msg);
    }

    void attach(uint32_t attachment, uint32_t type, uint32_t texture) {
        if (type == GL_TEXTURE_3D) {
            glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, type, texture, 1, 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, type, texture, 0);
        }
    }

    void draw_buffers(int32_t size, const GLenum *p_buffers) {
        glDrawBuffers(size, p_buffers);
    }

    void invoke(uint32_t key, uint32_t flags = 0) {
        /* Gen framebuffer is necessary. */
        gpu::frame &framebuffer {this->frame_map[key]};
        if (framebuffer.id_fbo == 0) glGenFramebuffers(1, &framebuffer.id_fbo);
        this->current_frame_info = key;

        /* Bind the current framebuffer and clear previous screen buffers. */
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id_fbo);
        if (flags != 0) glClear(flags);
    }

    void revoke(uint32_t flags = 0) {
        /* Unbind framebuffer and clean previous screen buffers. */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (flags != 0) glClear(flags);
        this->current_frame_info = 0;
    }

    void delete_buffer(uint32_t key) {
        if (!this->frame_map.count(key)) {
            return;
        }

        gpu::frame &frame {this->frame_map[key]};
        if (frame.id_fbo != 0) glDeleteFramebuffers(1, &frame.id_fbo);
        if (frame.id_renderbuffer != 0) glDeleteRenderbuffers(1, &frame.id_renderbuffer);
        if (frame.id_texture != 0) glDeleteTextures(1, &frame.id_texture);
        this->frame_map.erase(key);
    }

    void delete_buffers() {
        for (auto it {this->frame_map.begin()}; it != this->frame_map.end(); it = std::next(it)) {
            if (it->second.id_fbo != 0) glDeleteFramebuffers(1, &it->second.id_fbo);
            if (it->second.id_renderbuffer != 0) glDeleteRenderbuffers(1, &it->second.id_renderbuffer);
            if (it->second.id_texture != 0) glDeleteTextures(1, &it->second.id_texture);
        }

        this->frame_map.clear();
    }
};

#endif