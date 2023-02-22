#include "model.hpp"
#include "agk.hpp"

asset::model::model(std::string_view model_tag, std::string_view model_path, const glm::ivec4 &model_driver_mode, const std::function<void(buffering&, ::mesh::data&)> &injection_mixin) {
    this->tag = model_tag;
    this->path = model_path;
    this->mixin = injection_mixin;
    this->driver_mode[0] = model_driver_mode.x;
    this->driver_mode[1] = model_driver_mode.y;
    this->driver_mode[2] = model_driver_mode.z;
    this->driver_mode[3] = model_driver_mode.w;
}

asset::model::~model() {
    this->buffer.free_buffers();
}

void asset::model::on_create() {
    ::mesh::data mesh {};
    this->is_dead = agk::mesh::load(mesh, this->path);

    if (this->is_dead) {
        return;
    }

    this->buffer.invoke();

    if (mesh.contains(::mesh::type::vertex)) {
        auto &list {mesh.get_float_list(::mesh::type::vertex)};
        this->buffer.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(list), list.data(), this->driver_mode[0]);
        this->buffer.attach(0, 3);

        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = std::max((int32_t) (list.size() / 3), 1);

        glm::vec3 min {glm::vec3(std::numeric_limits<float>::max())};
        glm::vec3 max {glm::vec3(std::numeric_limits<float>::min())};

        for (int32_t it {}; it < this->buffer.stride[1]; it++) {
            const float *p_vec {&list.at(it * 3)};
            min.x = std::min(min.x, p_vec[0]);
            min.y = std::min(min.y, p_vec[1]);
            min.z = std::min(min.z, p_vec[2]);

            max.x = std::max(max.x, p_vec[0]);
            max.y = std::max(max.y, p_vec[1]);
            max.z = std::max(max.z, p_vec[2]);
        }

        this->axis_aligned_bounding_box = {min, max};
    }

    if (mesh.contains(::mesh::type::textcoord)) {
        auto &list {mesh.get_float_list(::mesh::type::textcoord)};
        this->buffer.bind(1, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(list), list.data(), this->driver_mode[1]);
        this->buffer.attach(1, 2);
    }

    if (mesh.contains(::mesh::type::normal)) {
        auto &list {mesh.get_float_list(::mesh::type::normal)};
        this->buffer.bind(2, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(list), list.data(), this->driver_mode[2]);
        this->buffer.attach(2, 3);
    }

    if (mesh.contains(::mesh::type::vertex, true)) {
        auto &list {mesh.get_uint_list(::mesh::type::vertex)};
        this->buffer.bind(2, {GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT});
        this->buffer.send<uint32_t>(sizeof(list), list.data(), this->driver_mode[2]);

        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = mesh.faces;
    }

    this->mixin(this->buffer, mesh);
    this->buffer.revoke();
}
