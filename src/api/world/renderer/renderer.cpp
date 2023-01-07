#include "api/world/renderer/renderer.hpp"
#include "api/world/environment/entity.hpp"
#include "api/world/model/model.hpp"
#include "api/world/environment/object.hpp"
#include "api/api.hpp"
#include "api/event/event.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

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

void renderer::process_terrain() {


}

void renderer::process_environment() {
    /* Terrain rendering section. */
    /* Here... */

    /* Object & entity rendering section. */

    auto camera {api::world::currentcamera()};
    glm::mat4 mvp {camera->get_perspective() * camera->get_view()};
    glm::mat4 mat4x4_model {};
    glm::mat3 cubic_normal_matrix {};

    shading::program *p_program_material_pbr {};
    api::shading::find("m.brdf.pbr", p_program_material_pbr);

    glUseProgram(p_program_material_pbr->id);
    p_program_material_pbr->set_uniform_vec3("CameraPosition", &camera->position[0]);

    entity *p_entity {};
    model *p_model {};
    object *p_object {};

    enums::material material_type {};
    int32_t current_light_loaded {};
    std::string light_index_tag {};

    for (world_feature *&p_world_feature : this->wf_draw_list) {
        if (p_world_feature == nullptr) {
            continue;
        }

        switch (p_world_feature->type) {
            case enums::type::entity: {
                p_entity = (entity*) p_world_feature;
                p_model = p_entity->p_model;
                break;
            }

            case enums::type::object: {
                p_object = (object*) p_world_feature;
                p_model = p_object->p_model;
                break;
            }

            default: {
                break;
            }
        }

        if (p_model == nullptr || p_object->p_material == nullptr) {
            continue;
        }

        mat4x4_model = glm::mat4(1.0f);
        mat4x4_model = glm::translate(mat4x4_model, p_object->position);
        mat4x4_model = glm::scale(mat4x4_model, p_object->scale);

        cubic_normal_matrix = glm::inverseTranspose(glm::mat3(mat4x4_model));
        p_program_material_pbr->set_uniform_mat3("MatrixNormal", &cubic_normal_matrix[0][0]);
        p_program_material_pbr->set_uniform_mat4("MatrixModel", &mat4x4_model[0][0]);

        mat4x4_model = mvp * mat4x4_model;
        p_program_material_pbr->set_uniform_mat4("MVP", &mat4x4_model[0][0]);

        material_type = p_object->p_material->get_type();
        p_program_material_pbr->set_uniform_bool("Material.Metal", material_type == enums::material::metal);
        p_program_material_pbr->set_uniform_float("Material.Rough", p_object->p_material->get_rough());
        p_program_material_pbr->set_uniform_vec3("Material.Color", &p_object->p_material->get_color()[0]);

        p_model->buffering.draw();
    }

    if (this->loaded_light_size != current_light_loaded) {
        this->loaded_light_size = current_light_loaded;
        p_program_material_pbr->set_uniform_int("LoadedLightLen", current_light_loaded);
    };

    glUseProgram(0);
}

void renderer::on_create() {
    feature::on_create();

    shading::program *p_program_material_pbr {new ::shading::program {}};
    api::shading::createprogram("m.brdf.pbr", p_program_material_pbr,  {
            {"./data/effects/material.brdf.pbr.vert", shading::stage::vertex},
            {"./data/effects/material.brdf.pbr.frag", shading::stage::fragment}
    });
}

void renderer::on_destroy() {
    feature::on_destroy();
}


void renderer::on_render() {
    feature::on_render();

    this->process_terrain();
    this->process_environment();
}

void renderer::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);

    switch (sdl_event.type) {
        case SDL_USEREVENT: {
            switch (sdl_event.user.code) {
                case event::WORLD_REFRESH_DRAW: {
                    this->on_event_refresh_draw(sdl_event);
                    break;
                }
            }

            break;
        }
    }
}

void renderer::on_event_refresh_draw(SDL_Event &sdl_event) {
    auto &world {api::world::get()};

    auto *p_to_remove {static_cast<bool*>(sdl_event.user.data1)};
    auto *p_wf_id {static_cast<int32_t*>(sdl_event.user.data2)};

    world_feature *p_world_feature {world->find(*p_wf_id)};
    if (*p_to_remove) {
        this->wf_draw_list.clear();
        if (p_world_feature != nullptr) p_world_feature->set_visible(enums::state::disable, false);

        for (world_feature *&p_world_features : world->wf_list) {
            if (p_world_features != nullptr && p_world_features->get_visible() == enums::state::enable) {
                this->wf_draw_list.push_back(p_world_features);
            }
        }
    } else if (p_world_feature != nullptr && p_world_feature->get_visible() != enums::state::enable) {
        this->wf_draw_list.push_back(p_world_feature);
        p_world_feature->set_visible(enums::state::enable, false);
    }

    delete p_to_remove;
    delete p_wf_id;
}
