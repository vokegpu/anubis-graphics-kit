#include "renderer.hpp"

model *renderer::add(std::string_view tag, mesh::data &mesh_data) {
    model *p_model {nullptr};

    if (this->find(tag, p_model)) {
        return p_model;
    }

    std::vector<float> f_list {};
    std::vector<uint32_t> i_list {};

    p_model = new model();
    p_model->id = static_cast<int32_t>(this->loaded_model_list.size()) + 1;
    this->loaded_model_list.push_back(p_model);
    this->model_register_map[tag.data()] = p_model->id;

    auto &buffering = p_model->buffering;
    buffering.invoke();
    buffering.stride[0] = 0;

    if (mesh_data.contains(mesh::type::vertex)) {
        f_list = mesh_data.get_float_list(mesh::type::vertex);
        buffering.bind({GL_ARRAY_BUFFER, GL_FLOAT});
        buffering.send(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffering.attach(3);
        buffering.stride[1] = f_list.size() / 3;
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

    if (mesh_data.contains(mesh::type::vertex, true)) {
        i_list = mesh_data.get_uint_list(mesh::type::vertex);
        buffering.bind({GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT});
        buffering.send(sizeof(uint32_t) * i_list.size(), i_list.data(), GL_STATIC_DRAW);
        buffering.attach(1);
    }

    buffering.revoke();
    return p_model;
}

bool renderer::find(std::string_view tag, model *& p_model) {
    int32_t index {this->model_register_map[tag.data()] - 1};
    if (index == -1 || index > this->loaded_model_list.size()) {
        return false;
    }

    p_model = this->loaded_model_list.at(index);
    return true;
}

bool renderer::contains(std::string_view tag) {
    int32_t index {this->model_register_map[tag.data()] - 1};
    return index != -1 && index < this->loaded_model_list.size();
}
