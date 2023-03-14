#include "model.hpp"

void model::load(std::string_view path) {
    if (this->tag != path) {
        this->compiled = true;
        this->tag = path;
        this->recompile();
    }
}

void model::recompile() {
    if (!this->compile) {
        return;
    }

    stream::mesh mesh {};
    if (akg::stream::load(mesh, this->path)) {
        /* keep flag compiled to false */
        return;
    }

    uint32_t buffers_driver_read_mode {this->static_buffers ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW};
    this->buffer.invoke();

    if (mesh.contains(stream::type::vertex)) {
        auto &list {mesh.get<float>(stream::type::vertex)};
        this->buffer.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float)*list.size(), this->size.data(), buffers_driver_read_mode);
        this->buffer.attach(0, 3);

        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = static_cast<int32_t>(list.size()) / 3;
        util::generate_aabb(this->aabb, mesh);
    }

    if (mesh.contains(stream::type::texcoord)) {
        auto &list {mesh.get<float>(stream::type::texcoord)};
        this->buffer.bind(1, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float)*list.size(), this->size.data(), buffers_driver_read_mode);
        this->buffer.attach(1, 2);
    }

    if (mesh.contains(stream::type::normal)) {
        auto &list {mesh.get<float>(stream::type::normal)};
        this->buffer.bind(2, {GL_ARRAY_BUFFER, GL_FLOAT});
        this->buffer.send<float>(sizeof(float)*list.size(), this->size.data(), buffers_driver_read_mode);
        this->buffer.attach(2, 3);
    }

    if (mesh.contains(stream::type::index)) {
        auto &list {mesh.get<uint32_t>(stream::type::index)};
        this->buffer.bind(3, {GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT});
        this->buffer.send<uint32_t>(sizeof(uint32_t)*list.size(), list.data(), buffers_driver_read_mode);

        this->buffer.stride[0] = 0;
        this->buffer.stride[1] = mesh.faces;
    }

    if (this->mixin) this->mixin(this->buffer, mesh);
    this->buffer.revoke();
    this->compiled = true;
}

bool model::is_compiled() {
    return this->compiled;
}

void model::on_destroy() {

}

void model::on_render() {

}