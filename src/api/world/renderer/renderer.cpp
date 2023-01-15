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

    auto &buffer = p_model->buffer;
    buffer.invoke();
    buffer.stride[0] = 0;

    if (mesh_data.contains(mesh::type::vertex)) {
        f_list = mesh_data.get_float_list(mesh::type::vertex);
        buffer.bind({GL_ARRAY_BUFFER, GL_FLOAT});
        buffer.send(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffer.attach(0, 3);
        buffer.stride[1] = f_list.size() / 3;
    }

    if (mesh_data.contains(mesh::type::textcoord)) {
        f_list = mesh_data.get_float_list(mesh::type::textcoord);
        buffer.bind({GL_ARRAY_BUFFER, GL_FLOAT});
        buffer.send(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffer.attach(1, 2);
    }

    if (mesh_data.contains(mesh::type::normal)) {
        f_list = mesh_data.get_float_list(mesh::type::normal);
        buffer.bind({GL_ARRAY_BUFFER, GL_FLOAT});
        buffer.send(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffer.attach(2, 3);
    }

    if (mesh_data.contains(mesh::type::vertex, true)) {
        i_list = mesh_data.get_uint_list(mesh::type::vertex);
        buffer.bind({GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT});
        buffer.send(sizeof(uint32_t) * i_list.size(), i_list.data(), GL_STATIC_DRAW);
        buffer.stride[0] = mesh_data.faces;
        buffer.stride[1] = 0;
    }

    buffer.revoke();
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
    shading::program *p_program_terrain_pbr {};
    api::shading::find("terrain.pbr", p_program_terrain_pbr);

    glm::mat4 mat4x4_model {};
    glUseProgram(p_program_terrain_pbr->id);

    p_program_terrain_pbr->set_uniform_vec2("Fog.Distance", &this->config_fog_distance.get_value()[0]);
    p_program_terrain_pbr->set_uniform_vec3("Fog.Color", &this->config_fog_color.get_value()[0]);
    p_program_terrain_pbr->set_uniform_int("ActiveTexture", 0);

    glActiveTexture(GL_TEXTURE0);

    for (chunk *&p_chunks : this->wf_chunk_draw_list) {
        if (p_chunks == nullptr || !p_chunks->is_processed()) {
            continue;
        }

        glBindTexture(GL_TEXTURE_2D, p_chunks->heightmap);

        mat4x4_model = glm::mat4(1);
        mat4x4_model = glm::translate(mat4x4_model, p_chunks->position);
        mat4x4_model = glm::scale(mat4x4_model, p_chunks->scale);

        mat4x4_model = this->mat4x4_mvp * mat4x4_model;
        p_program_terrain_pbr->set_uniform_mat4("MVP", &mat4x4_model[0][0]);

        p_chunks->buffer.invoke();
        p_chunks->buffer.draw();
        p_chunks->buffer.revoke();
    }

    glUseProgram(0);
}

void renderer::process_environment() {
    auto camera {api::world::currentcamera()};

    glm::mat4 mat4x4_model {};
    glm::mat3 cubic_normal_matrix {};

    shading::program *p_program_m_brdf_pbr {};
    api::shading::find("m.brdf.pbr", p_program_m_brdf_pbr);

    glUseProgram(p_program_m_brdf_pbr->id);

    entity *p_entity {};
    model *p_model {};
    object *p_object {};

    enums::material material_type {};
    int32_t current_light_loaded {};
    std::string light_index_tag {};

    for (world_feature *&p_world_feature : this->wf_env_draw_list) {
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
        mat4x4_model = glm::rotate(mat4x4_model, p_object->rotation.x, {1, 0, 0});
        mat4x4_model = glm::rotate(mat4x4_model, p_object->rotation.y, {0, 1, 0});
        mat4x4_model = glm::rotate(mat4x4_model, p_object->rotation.z, {0, 0, 1});
        mat4x4_model = glm::scale(mat4x4_model, p_object->scale);

        cubic_normal_matrix = glm::inverseTranspose(glm::mat3(mat4x4_model));
        p_program_m_brdf_pbr->set_uniform_mat3("MatrixNormal", &cubic_normal_matrix[0][0]);
        p_program_m_brdf_pbr->set_uniform_mat4("MatrixModel", &mat4x4_model[0][0]);

        mat4x4_model = this->mat4x4_mvp * mat4x4_model;
        p_program_m_brdf_pbr->set_uniform_mat4("MVP", &mat4x4_model[0][0]);

        material_type = p_object->p_material->get_type();
        p_program_m_brdf_pbr->set_uniform_bool("Material.Metal", material_type == enums::material::metal);
        p_program_m_brdf_pbr->set_uniform_float("Material.Rough", p_object->p_material->get_rough());
        p_program_m_brdf_pbr->set_uniform_vec3("Material.Color", &p_object->p_material->get_color()[0]);

        p_model->buffer.invoke();
        p_model->buffer.draw();
        p_model->buffer.revoke();
    }

    if (this->loaded_light_size != current_light_loaded) {
        this->loaded_light_size = current_light_loaded;
        p_program_m_brdf_pbr->set_uniform_int("LoadedLightLen", current_light_loaded);
    };

    glUseProgram(0);
}

void renderer::on_create() {
    feature::on_create();

    shading::program *p_program_m_brdf_pbr {new ::shading::program {}};
    api::shading::createprogram("m.brdf.pbr", p_program_m_brdf_pbr, {
            {"./data/effects/material.brdf.pbr.vert", shading::stage::vertex},
            {"./data/effects/material.brdf.pbr.frag", shading::stage::fragment}
    });

    shading::program *p_program_terrain_pbr {new ::shading::program {}};
    api::shading::createprogram("terrain.pbr", p_program_terrain_pbr, {
            {"./data/effects/terrain.pbr.vert", shading::stage::vertex},
            {"./data/effects/terrain.pbr.frag", shading::stage::fragment},
            {"./data/effects/terrain.pbr.tesc", shading::stage::tesscontrol},
            {"./data/effects/terrain.pbr.tese", shading::stage::tessevaluation}
    });

    this->config_fog_distance.set_value({0.0f, 512.0f});
    this->config_fog_color.set_value({1.0f, 1.0f, 1.0f});
}

void renderer::on_destroy() {
    feature::on_destroy();
}

void renderer::on_render() {
    feature::on_render();

    if (this->update_disabled_chunks) {
        this->update_disabled_chunks = false;
        this->wf_chunk_draw_list.clear();

        for (chunk *&p_chunks : api::world::get()->loaded_chunk_list) {
            if (p_chunks != nullptr && p_chunks->is_processed()) {
                this->wf_chunk_draw_list.push_back(p_chunks);
            }
        }
    }

    /* Prepare matrices to render the world. */
    auto &p_camera {api::world::currentcamera()};
    this->mat4x4_mvp = p_camera->get_perspective() * p_camera->get_view();

    this->process_terrain();
    this->process_environment();
}

void renderer::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);

    switch (sdl_event.type) {
        case SDL_USEREVENT: {
            switch (sdl_event.user.code) {
                case event::WORLD_REFRESH_ENVIRONMENT: {
                    this->on_event_refresh_environment(sdl_event);
                    break;
                }

                case event::WORLD_REFRESH_CHUNK: {
                    this->on_event_refresh_chunk(sdl_event);
                    break;
                }
            }

            break;
        }
    }
}

void renderer::on_event_refresh_environment(SDL_Event &sdl_event) {
    auto &world {api::world::get()};

    auto *p_to_remove {static_cast<bool*>(sdl_event.user.data1)};
    auto *p_wf_id {static_cast<int32_t*>(sdl_event.user.data2)};

    world_feature *p_world_feature {world->find_env_wf(*p_wf_id)};
    if (*p_to_remove) {
        this->wf_env_draw_list.clear();
        if (p_world_feature != nullptr) p_world_feature->set_visible(enums::state::disable, false);

        for (world_feature *&p_world_features : world->wf_list) {
            if (p_world_features != nullptr && p_world_features->get_visible() == enums::state::enable) {
                this->wf_env_draw_list.push_back(p_world_features);
            }
        }
    } else if (p_world_feature != nullptr && p_world_feature->get_visible() != enums::state::enable) {
        this->wf_env_draw_list.push_back(p_world_feature);
        p_world_feature->set_visible(enums::state::enable, false);
    }

    delete p_to_remove;
    delete p_wf_id;
}

void renderer::on_event_refresh_chunk(SDL_Event &sdl_event) {
    auto *p_string_chunk_tag {static_cast<std::string*>(sdl_event.user.data1)};
    auto *&p_world {api::world::get()};
    auto *p_chunk {p_world->find_chunk_wf(*p_string_chunk_tag)};

    if (p_chunk != nullptr && p_chunk->is_processed()) {
        this->add(p_chunk);
    }

    delete p_string_chunk_tag;
}

void renderer::add(chunk *p_chunk) {
    if (p_chunk == nullptr || !p_chunk->is_processed()) {
        return;
    }

    auto &mesh_chunk {api::world::get()->chunk_mesh_data};

    auto &buffer {p_chunk->buffer};
    auto &v {mesh_chunk.get_float_list(mesh::type::vertex)};
    auto &t {mesh_chunk.get_float_list(mesh::type::textcoord)};
    auto &c {mesh_chunk.get_float_list(mesh::type::color)};
    auto &i {mesh_chunk.get_uint_list(mesh::type::vertex)};

    buffer.invoke();
    buffer.bind({GL_ARRAY_BUFFER, GL_FLOAT});
    buffer.send(sizeof(float) * v.size(), v.data(), GL_STATIC_DRAW);
    buffer.attach(0, 3);

    buffer.bind({GL_ARRAY_BUFFER, GL_FLOAT});
    buffer.send(sizeof(float) * t.size(), t.data(), GL_STATIC_DRAW);
    buffer.attach(1, 3);

    buffer.stride[0] = 0;
    buffer.stride[1] = 4 * mesh_chunk.faces;
    buffer.tessellation(4);

    buffer.revoke();
    this->wf_chunk_draw_list.push_back(p_chunk);
}

void renderer::refresh() {
    this->update_disabled_chunks = true;
}
