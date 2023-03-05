#include "model.hpp"
#include "agk.hpp"

asset::model::model(std::string_view model_tag, std::string_view model_path, const glm::ivec4 &model_driver_mode, const std::function<void(buffering&, ::mesh::data&)> &injection_mixin) {
    this->tag += "models/";
    this->tag += this->name = model_tag;

    this->path = model_path;
    this->mixin = injection_mixin;
    this->driver_mode[0] = model_driver_mode.x;
    this->driver_mode[1] = model_driver_mode.y;
    this->driver_mode[2] = model_driver_mode.z;
    this->driver_mode[3] = model_driver_mode.w;
}

asset::model::model(std::string_view model_tag, const std::function<void(buffering &, ::mesh::data &)> &injection_mixin) {
    this->tag += "models/";
    this->tag += model_tag;
    this->mixin = injection_mixin;
}

asset::model::~model() {
    this->buffer.delete_buffers();
}

void asset::model::on_create() {
    ::mesh::data mesh {};

    if (!this->path.empty()) {
        this->is_dead = agk::mesh::load(mesh, this->path);
    }

    if (this->is_dead) {
        return;
    }

    this->buffer.invoke();

    if (mesh.contains(::mesh::type::vertex)) {
        auto &list {mesh.get_float_list(::mesh::type::vertex)};
        this->buffer.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float) * list.size(), list.data(), this->driver_mode[0]);
        this->buffer.attach(0, 3);

        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = (int32_t) list.size() / 3;
        util::generate_aabb(this->aabb, mesh);
    }

    if (mesh.contains(::mesh::type::textcoord)) {
        auto &list {mesh.get_float_list(::mesh::type::textcoord)};
        this->buffer.bind(1, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float) * list.size(), list.data(), this->driver_mode[1]);
        this->buffer.attach(1, 2);
    }

    if (mesh.contains(::mesh::type::normal)) {
        auto &list {mesh.get_float_list(::mesh::type::normal)};
        this->buffer.bind(2, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float) * list.size(), list.data(), this->driver_mode[2]);
        this->buffer.attach(2, 3);
    }

    if (mesh.contains(::mesh::type::vertex, true)) {
        auto &list {mesh.get_uint_list(::mesh::type::vertex)};
        this->buffer.bind(3, {GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT});
        this->buffer.send<uint32_t>(sizeof(float) * list.size(), list.data(), this->driver_mode[2]);

        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = mesh.faces;
    }

    if (this->mixin) {
        this->mixin(this->buffer, mesh);
    }

    switch (mesh.format) {
        case mesh::format::wavefrontobj: {
            std::string mm {};
            const glm::vec4 zero4f {};

            for (std::string &mtl : mesh.wavefront.mtllib_newmtl_list) {
                for (auto &[key, value] : mesh.wavefront.mtllib_map[mtl].strings) {
                    mm.clear();
                    mm += this->name;
                    mm += '.';
                    mm += key;
                    mm += '.';
                    mm += value;

                    ::asset::texture<uint8_t> *p_mtl_asset {new ::asset::texture<uint8_t> {mm, "./data/textures/" + value, {GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_RGB, GL_RGB}, [](gpu::texture &texture, bool &mipmap) {
                        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                        glTexParameteri(texture.type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                        glTexParameteri(texture.type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                        glTexParameteri(texture.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(texture.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

                        mipmap = true;
                    }}};

                    agk::asset::load(p_mtl_asset);
                    this->linked_mtl_list.push_back(p_mtl_asset->tag);
                }
            }

            this->mtllib_wavefront = mesh.wavefront;
            break;
        }

        default: {
            break;
        }
    }

    this->buffer.revoke();
}

std::vector<std::string> &asset::model::get_linked_mtl_list() {
    return this->linked_mtl_list;
}