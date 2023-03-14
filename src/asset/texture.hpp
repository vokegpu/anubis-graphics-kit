#ifndef AGK_ASSET_TEXTURE_H
#define AGK_ASSET_TEXTURE_H

#include "gpu/tools.hpp"
#include <functional>
#include "stb/stb_image.h"
#include "core/imodule.hpp"
#include <map>

namespace asset {
    template<typename t>
    class texture : public imodule {
    protected:
        std::map<std::string, glm::vec4> atlas_map {};
        gpu::texture gpu_side {};
        t *cpu_side {};
        std::function<void(gpu::texture&, bool&)> mixin {};
    public:
        explicit texture(std::string_view texture_name, std::string_view path, const glm::ivec4 &texture_info, const std::function<void(gpu::texture &, bool &)> &injection_mixin = {}) {
            this->tag += "textures/";
            this->tag += texture_name;

            this->mixin = injection_mixin;
            this->gpu_side.primitive = texture_info.y;
            this->gpu_side.type = texture_info.x;
            this->gpu_side.format = texture_info.z;
            this->gpu_side.channel = texture_info.w;

            int32_t channel {};
            int32_t w {}, h {};

            this->cpu_side = stbi_load(path.data(), &w, &h, &channel, 0);
            this->gpu_side.w = w;
            this->gpu_side.h = h;

            this->is_dead = !this->cpu_side;
            glGenTextures(1, &this->gpu_side.id);
        }

        ~texture() {
            if (this->gpu_side.id != 0) {
                glDeleteTextures(1, &this->gpu_side.id);
            }
        }

        void add(const std::string &sub_texture, glm::vec4 tex_coord) {
            if (glm::length(tex_coord) > 1.0f) {
                tex_coord /= glm::vec4 {this->gpu_side.w, this->gpu_side.h, this->gpu_side.w, this->gpu_side.h};
            }

            this->atlas_map.insert({sub_texture, tex_coord});
        }

        void read_cpu_side() {
            if (this->cpu_side != nullptr) {
                this->free_cpu_side();
            }

            this->cpu_side = new t[this->gpu_side.w * this->gpu_side.h * this->gpu_side.w * this->gpu_side.channel];
            gpu::read<t>(this->gpu_side, this->cpu_side);
        }

        void invoke() const {
            glBindTexture(this->gpu_side.type, this->gpu_side.id);
        }

        void attach(std::string_view uniform_structure, std::string_view uniform_texcoord, ::asset::shader *&p_shader) {
            std::string uniform_name {};
            int32_t index {};

            for (auto &[key, value] : this->atlas_map) {
                uniform_name.clear();
                uniform_name += uniform_structure;
                uniform_name += '[';
                uniform_name += std::to_string(index++);
                uniform_name += ']';
                uniform_name += '.';
                uniform_name += uniform_texcoord;

                p_shader->set_uniform_vec4(uniform_name, &value[0]);
            }
        }

        void revoke() const {
            glBindTexture(this->gpu_side.type, 0);
        }

        void free_cpu_side() {
            delete this->cpu_side;
        }

        gpu::texture &gpu_side_data() {
            return this->gpu_side;
        }

        t *&cpu_side_data() {
            return this->cpu_side;
        }

        void on_create() override {
            if (this->is_dead) {
                glDeleteTextures(1, &this->gpu_side.id);
                return;
            }

            glBindTexture(this->gpu_side.type, this->gpu_side.id);

            bool mipmap {};
            if (this->mixin) {
                this->mixin(this->gpu_side, mipmap);
            }

            switch (this->gpu_side.type) {
                case GL_TEXTURE_1D: {
                    glTexImage1D(this->gpu_side.type, 0, this->gpu_side.format, this->gpu_side.w, 0, this->gpu_side.channel, this->gpu_side.primitive, this->cpu_side);
                    break;
                }

                case GL_TEXTURE_2D: {
                    glTexImage2D(this->gpu_side.type, 0, this->gpu_side.format, this->gpu_side.w, this->gpu_side.h, 0, this->gpu_side.channel, this->gpu_side.primitive, this->cpu_side);
                    break;
                }

                case GL_TEXTURE_3D: {
                    glTexImage3D(this->gpu_side.type, 0, this->gpu_side.format, this->gpu_side.w, this->gpu_side.h, this->gpu_side.d, 0, this->gpu_side.channel, this->gpu_side.primitive, this->cpu_side);
                    break;
                }
            }

            if (mipmap) {
                glGenerateMipmap(this->gpu_side.type);
            }

            glBindTexture(this->gpu_side.type, 0);
            this->free_cpu_side();
        }
    };
}

#endif