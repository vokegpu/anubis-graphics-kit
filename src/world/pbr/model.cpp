#include "model.hpp"
#include "agk.hpp"

model::model(stream::mesh &_mesh) {
    this->mesh = _mesh;
}

model::~model() {
    this->buffer.delete_buffers();
}

void model::load(stream::mesh &_mesh) {
    this->compiled = true;
    this->mesh = _mesh;
    this->recompile();
}

void model::recompile() {
    if (this->compiled) {
        return;
    }

    int32_t buffers_driver_read_mode {this->static_buffers ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW};
    this->buffer.invoke();

    if (this->mesh.contains(stream::type::vertex)) {
        auto &list {this->mesh.get_float_list(stream::type::vertex)};
        this->buffer.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float)*list.size(), list.data(), buffers_driver_read_mode);
        this->buffer.attach(0, 3);

        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = static_cast<int32_t>(list.size()) / 3;
        util::generateaabb(this->aabb, this->mesh);
    }

    if (this->mesh.contains(stream::type::texcoord)) {
        auto &list {this->mesh.get_float_list(stream::type::texcoord)};
        this->buffer.bind(1, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float)*list.size(), list.data(), buffers_driver_read_mode);
        this->buffer.attach(1, 2);
    }

    if (this->mesh.contains(stream::type::normal)) {
        auto &list {this->mesh.get_float_list(stream::type::normal)};
        this->buffer.bind(2, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float)*list.size(), list.data(), buffers_driver_read_mode);
        this->buffer.attach(2, 3);
    }

    if (this->mesh.contains(stream::type::index)) {
        auto &list {this->mesh.get_uint_list(stream::type::index)};
        this->buffer.bind(3, {GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT});
        this->buffer.send<uint32_t>(sizeof(uint32_t)*list.size(), list.data(), buffers_driver_read_mode);
        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = list.size();
    }

    this->buffer.revoke();
    this->compiled = true;
    util::log("Compiled model buffers: " + std::to_string(this->mesh.faces) + " faces");
}

bool model::is_compiled() {
    return this->compiled;
}