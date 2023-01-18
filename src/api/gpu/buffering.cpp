#include "buffering.hpp"
#include "api/util/env.hpp"

uint32_t buffering::current_type_bind[2] {};

void buffering::bind(const glm::ivec2 &buffer_type) {
    uint32_t buffer {};
    if (buffer_type.x == GL_ELEMENT_ARRAY_BUFFER) {
        if (this->buffer_ebo == 0) glGenBuffers(1, &this->buffer_ebo);
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

void buffering::attach(int32_t location, int32_t vec, const glm::ivec2 &array_stride) {
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, vec, buffering::current_type_bind[1], GL_FALSE, array_stride.x, (void*) static_cast<uint64_t>(array_stride.y));
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
    switch (this->type) {
        case buffering::type::direct: {
            if (this->buffer_ebo != 0) {
                glDrawElements(this->primitive, stride[0], GL_UNSIGNED_INT, (void*) stride[1]);
            } else {
                glDrawArrays(this->primitive, stride[0], stride[1]);
            }
            break;
        }

        case buffering::type::instanced: {
            if (this->buffer_ebo != 0) {
                glDrawElementsInstanced(this->primitive, stride[1], GL_UNSIGNED_INT, nullptr, stride[2]);
            } else {
                glDrawArraysInstanced(this->primitive, stride[0], stride[1], stride[2]);
            }
            break;
        }
    }
}

void buffering::free_buffers() {
    if (this->buffer_vao != 0) glDeleteVertexArrays(1, &this->buffer_vao);
    if (this->buffer_ebo != 0) glDeleteBuffers(1, &this->buffer_ebo);
    if (!this->buffer_list.empty()) glDeleteBuffers((int32_t) this->buffer_list.size(), this->buffer_list.data());
}

void buffering::tessellation(int32_t patches) {
    glPatchParameteri(GL_PATCH_VERTICES, patches);
    this->primitive = GL_PATCHES;
}

buffering::~buffering() {
    this->free_buffers();
}

uint32_t framebuffering::get_texture() {
    return this->buffer_texture;
}

uint32_t framebuffering::get_fbo() {
    return this->buffer_fbo;
}

uint32_t framebuffering::get_depth_buffer() {
    return this->buffer_renderbuffer;
}

void framebuffering::send(int32_t width, int32_t height) {
    if (this->w == width && this->h == height) {
        return;
    }

    this->w = width;
    this->h = height;

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->buffer_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->w, this->h, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->buffer_renderbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->buffer_texture, 0);

    GLenum framebuffer_check {glCheckFramebufferStatus(GL_FRAMEBUFFER)};
    GLenum buffers {GL_COLOR_ATTACHMENT0};

    glDrawBuffers(1, &buffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (framebuffer_check != GL_FRAMEBUFFER_COMPLETE) {
        util::log("Failed to generate framebuffer!");
    } else {
        util::log("Framebuffer compiled & attached to GPU!");
    }
}

void framebuffering::invoke() {
    if (this->buffer_fbo == 0) {
        glGenFramebuffers(1, &this->buffer_fbo);
    }

    if (this->buffer_renderbuffer == 0) {
        glGenRenderbuffers(1, &this->buffer_renderbuffer);
    }

    if (this->buffer_texture == 0) {
        glGenTextures(1, &this->buffer_texture);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, this->buffer_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void framebuffering::revoke() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void framebuffering::free_buffers() {
    if (this->buffer_texture != 0) glDeleteTextures(1, &this->buffer_texture);
    if (this->buffer_renderbuffer != 0) glDeleteFramebuffers(1, &this->buffer_renderbuffer);
    if (this->buffer_renderbuffer != 0) glDeleteRenderbuffers(1, &this->buffer_renderbuffer);
}

framebuffering::~framebuffering() {
    this->free_buffers();
}
