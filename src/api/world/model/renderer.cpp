#include "renderer.hpp"


model *renderer::add(std::string_view tag, mesh::data &mesh_data) {
    model *p_model {this->find(tag)};
    if (p_model != nullptr) {
        return p_model;
    }

    std::vector<float> f_list {};
    std::vector<uint32_t> i_list {};

    p_model = new model {};
    p_model->id = static_cast<int32_t>(this->loaded_model_list.size()) + 1;

    auto &buffering = p_model->buffering;
    buffering.invoke();

    if (mesh_data.contains(mesh::type::vertex)) {
        f_list = mesh_data.get_float_list(mesh::type::vertex);
        buffering.bind({GL_ARRAY_BUFFER, GL_FLOAT});
        buffering.send(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffering.attach(3);
    }

    if (mesh_data.contains(mesh::type::textcoord)) {
        f_list = mesh_data.get_float_list(mesh::type::textcoord);
        buffering.bind({GL_ARRAY_BUFFER, GL_FLOAT});
        buffering.send(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffering.attach(2);
    }

    if (mesh_data.contains(mesh::type::normal)) {
        f_list = mesh_data.get_float_list(mesh::type::normal);
        buffering.bind({GL_ARRAY_BUFFER, GL_FLOAT});
        buffering.send(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffering.attach(3);
    }

    if (mesh_data.contains(mesh::type::vertex)) {
        i_list = mesh_data.get_indexing_list(mesh::type::vertex);
        buffering.bind({GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT});
        buffering.send(sizeof(uint32_t) * i_list.size(), i_list.data(), GL_STATIC_DRAW);
        buffering.attach(1);
    }

    buffering.revoke();
    return p_model;
}

model *renderer::find(std::string_view tag) {
    int32_t index {this->model_register_map[tag.data()] - 1};
    if (index == -1 || index > this->loaded_model_list.size()) {
        return nullptr;
    }

    return this->loaded_model_list.at(index);
}

bool renderer::contains(std::string_view tag) {
    int32_t index {this->model_register_map[tag.data()] - 1};
    return index != -1 && index < this->loaded_model_list.size();
}
