#ifndef AGK_API_ASSET_IMPL_TEXTURE_H
#define AGK_API_ASSET_IMPL_TEXTURE_H

#include "api/gpu/tools.hpp"
#include <functional>
#include "stb/stb_image.h"
#include <map>

namespace asset {
    template<typename t>
    class texture : public feature {
    protected:
        std::map<std::string, glm::vec4> atlas_map {};
        gpu::texture gpu_side {};
        t *cpu_side {};
        std::function<void(gpu::texture&, bool&)> mixin {};
    public:
        explicit texture(std::string_view texture_name, std::string_view path, int32_t primitive, const std::function<void(gpu::texture&, bool&)> &fun_mixin) {
            this->tag = texture_name;
            this->mixin = fun_mixin;
            this->gpu_side.primitive = primitive;

            int32_t channel {};
            int32_t w {}, h {};

            this->cpu_side = stbi_load(path.data(), &w, &h, &channel, 0);
            this->gpu_side.w = w;
            this->gpu_side.h = h;

            switch (channel) {
                case 1: {
                    this->gpu_side.channel = GL_RED;
                    break;
                }

                case 2: {
                    this->gpu_side.channel = GL_RG;
                    break;
                }

                case 3: {
                    this->gpu_side.channel = GL_RGB;
                    break;
                }

                case 4: {
                    this->gpu_side.channel = GL_RGBA;
                    break;
                }

                default: {
                    break;
                }
            }
        }

        ~texture() {
            if (this->gpu_side.id != 0) {
                glDeleteTextures(1, &this->gpu_side.id);
            }
        }

        void add(const std::string &sub_texture, const glm::vec4 &tex_coord) {
            this->atlas_map[sub_texture] = tex_coord;
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

        void attach(std::string_view uniform, ::asset::shader *&p_shader) {
            std::string uniform_name {};
            int32_t index {};

            for (auto &[key, value] : this->atlas_map) {
                uniform_name.clear();
                uniform_name += uniform;
                uniform_name += '[';
                uniform_name += std::to_string(index++);
                uniform_name += ']';

                p_shader->set_uniform_vec4(uniform_name, &value[0]);
                util::log(uniform_name);
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

        t &cpu_side_data() {
            return this->cpu_side;
        }

        void on_create() override {
            glGenTextures(1, &this->gpu_side.id);
            glBindTexture(this->gpu_side.type, this->gpu_side.id);

            bool mipmap {};
            this->mixin(this->gpu_side, mipmap);

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