#ifndef AGK_GPU_TEXTURE_H
#define AGK_GPU_TEXTURE_H

#include "GL/glew.h"
#include <unordered_map>
#include "gpu.hpp"
#include <glm/glm.hpp>

class texturing {
protected:
    std::unordered_map<uint32_t, gpu::texture> texture_map {};
public:
    uint32_t current_texture_info[2] {};

    explicit texturing() = default;
    ~texturing() = default;;

    gpu::texture &operator[](uint32_t key) {
        return this->texture_map[key];
    }

    void invoke_active(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
    }

    void invoke(uint32_t key, const glm::ivec2 &texture_info) {
        this->current_texture_info[0] = key;
        this->current_texture_info[1] = texture_info.x;

        gpu::texture &texture {this->texture_map[key]};
        texture.type = texture_info.x;
        texture.primitive = texture_info.y;

        /* Gen texture and bind it. */
        if (texture.id == 0) glGenTextures(1, &texture.id);
        glBindTexture(texture.type, texture.id);
    }

    void revoke() {
        /* Unbind all textures. */
        glBindTexture(this->current_texture_info[1], 0);
        this->current_texture_info[0] = 0;
        this->current_texture_info[1] = 0;
    }

    void delete_buffers() {
        for (auto it {this->texture_map.begin()}; it != this->texture_map.end(); it = std::next(it)) {
            if (it->second.id != 0) glDeleteTextures(1, &it->second.id);
        }

        this->texture_map.clear();
    }

    void delete_buffer(uint32_t key) {
        if (!this->texture_map.count(key)) {
            return;
        }

        gpu::texture &texture {this->texture_map[key]};
        if (texture.id != 0) glDeleteTextures(1, &texture.id);
        this->texture_map.erase(key);
    }

    template<typename t>
    void send(const glm::ivec3 &in_dimension, const t *p_data, const glm::ivec2 &in_format, const glm::ivec3 &in_filter = {GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE}) {
        gpu::texture &texture {this->texture_map[this->current_texture_info[0]]};

        texture.w = in_dimension.x;
        texture.h = in_dimension.y;
        texture.d = in_dimension.z;

        texture.format = in_format.x;
        texture.channel = in_format.y;

        /* Pass image filtering. */
        glTexParameteri(texture.type, GL_TEXTURE_MIN_FILTER, in_filter.x);
        glTexParameteri(texture.type, GL_TEXTURE_MAG_FILTER, in_filter.y);

        glTexParameteri(texture.type, GL_TEXTURE_WRAP_S, in_filter.z);
        glTexParameteri(texture.type, GL_TEXTURE_WRAP_T, in_filter.z);

        switch (texture.type) {
            case GL_TEXTURE_1D: {
                glTexImage1D(texture.type, 0, texture.format, texture.w, 0, texture.channel, texture.primitive, p_data);
                break;
            }

            case GL_TEXTURE_2D: {
                glTexImage2D(texture.type, 0, texture.format, texture.w, texture.h, 0, texture.channel, texture.primitive, p_data);
                break;
            }

            case GL_TEXTURE_3D: {
                glTexParameteri(texture.type, GL_TEXTURE_WRAP_R, in_filter.z);
                glTexImage3D(texture.type, 0, texture.format, texture.w, texture.h, texture.d, 0, texture.channel, texture.primitive, p_data);
                break;
            }
        }
    }

    void generate_mipmap() {
        glGenerateMipmap(this->current_texture_info[1]);
    }

    template<typename t>
    t *get(t *p_data) {
        gpu::texture &texture {this->texture_map[this->current_texture_info[0]]};
        glGetTexImage(texture.type, 0, texture.channel, texture.primitive, p_data);
        return p_data;
    }

    template<typename t>
    std::vector<t> &get(std::vector<t> &data) {
        gpu::texture &texture {this->texture_map[this->current_texture_info[0]]};

        /* Get the new texture data from GPU. */
        int64_t size {texture.w * texture.h * texture.d * gpu::channels(texture)};
        if (data.size() != size) {
            data.resize(size);
        }

        gpu::read<t>(texture, data.data());
        return data;
    }

    int64_t size() {
        return (int64_t) this->texture_map.size();
    }
};

#endif