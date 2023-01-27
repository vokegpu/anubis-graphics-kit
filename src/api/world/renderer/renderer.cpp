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

    this->buffer_chunk.invoke();
    glActiveTexture(GL_TEXTURE0);

    for (chunk *&p_chunks : this->wf_chunk_draw_list) {
        if (p_chunks == nullptr || !p_chunks->is_processed()) {
            continue;
        }

        glBindTexture(GL_TEXTURE_2D, p_chunks->texture);

        mat4x4_model = glm::mat4(1);
        mat4x4_model = glm::translate(mat4x4_model, p_chunks->position);
        mat4x4_model = glm::scale(mat4x4_model, p_chunks->scale);

        mat4x4_model = this->mat4x4_mvp * mat4x4_model;
        p_program_terrain_pbr->set_uniform_mat4("MVP", &mat4x4_model[0][0]);
        p_program_terrain_pbr->set_uniform_mat4("MatrixViewModel", &mat4x4_model[0][0]);
        
        this->buffer_chunk.draw();
    }

    this->buffer_chunk.revoke();
    glUseProgram(0);
}

void renderer::process_environment() {
    auto camera {api::world::current_camera()};

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

void renderer::process_post_processing() {
    /* Invoke framebuffer and start collect screen buffers. */
    this->framebuffer_post_processing.invoke(0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->process_terrain();
    this->process_environment();

    /* Revoke all buffers from frame. */
    this->framebuffer_post_processing.revoke(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Draw the current frame buffer. */
    uint32_t texture {this->framebuffer_post_processing[0].id_texture};
    float ave_lum {};

    if (this->config_hdr.get_value()) {
        int32_t size {api::app.screen_width * api::app.screen_height};

        this->parallel_post_processing.dispatch_groups[0] = 32;
        this->parallel_post_processing.dispatch_groups[0] = 32;
        this->parallel_post_processing.dimension[0] = api::app.screen_width;
        this->parallel_post_processing.dimension[1] = api::app.screen_height;

        /* Pass current framebuffer to HDR luminance texture.  */
        this->parallel_post_processing.invoke();
        this->texture_post_processing.invoke(0, {GL_TEXTURE_2D, GL_FLOAT});
        this->texture_post_processing[HDR_FRAMEBUFFER_TEXTURE].id = texture;
        this->texture_post_processing[HDR_FRAMEBUFFER_TEXTURE].w = api::app.screen_width;
        this->texture_post_processing[HDR_FRAMEBUFFER_TEXTURE].h = api::app.screen_height;
        this->texture_post_processing[HDR_FRAMEBUFFER_TEXTURE].channel = GL_RGBA;
        this->texture_post_processing[HDR_FRAMEBUFFER_TEXTURE].format = GL_RGBA32F;
        this->parallel_post_processing.attach(0, this->texture_post_processing[HDR_FRAMEBUFFER_TEXTURE], GL_READ_ONLY);

        this->texture_post_processing.invoke(HDR_LUMINANCE_TEXTURE, {GL_TEXTURE_2D, GL_FLOAT});
        this->texture_post_processing.send<float>({1, 1, 0}, nullptr, {GL_R32F, GL_RED});
        this->parallel_post_processing.attach(1, this->texture_post_processing[HDR_LUMINANCE_TEXTURE], GL_READ_WRITE);

        this->parallel_post_processing.dispatch();

        /* Read luminance texture for get the sum of all pixels log(Lum + 0.0001f) */
        this->texture_post_processing.invoke(HDR_LUMINANCE_TEXTURE, {GL_TEXTURE_2D, GL_FLOAT});
        std::vector<float> luminance_image_list {};
        this->texture_post_processing.get<float>(luminance_image_list);
        ave_lum = expf(luminance_image_list[0] / static_cast<float>(size));
        this->parallel_post_processing.revoke();
    }

    this->immshape_post_processing.invoke();
    this->immshape_post_processing.p_program->set_uniform_float("AveLuminance", ave_lum);
    this->immshape_post_processing.p_program->set_uniform_float("Exposure", this->config_hdr_exposure.get_value());
    this->immshape_post_processing.p_program->set_uniform_bool("Effects.HighDynamicRange", this->config_hdr.get_value());
    this->immshape_post_processing.bind_texture(texture);
    this->immshape_post_processing.draw({0, 0, api::app.screen_width, api::app.screen_height}, {1.0f, 1.0f, 1.0f, 1.0f});
    this->immshape_post_processing.revoke();
}

void renderer::on_create() {
    feature::on_create();

    api::shading::create_program("m.brdf.pbr", new ::shading::program {}, {
            {"./data/effects/material.brdf.pbr.vert", ::shading::stage::vertex},
            {"./data/effects/material.brdf.pbr.frag", ::shading::stage::fragment}
    });

    api::shading::create_program("terrain.pbr", new ::shading::program {}, {
            {"./data/effects/terrain.pbr.vert", ::shading::stage::vertex},
            {"./data/effects/terrain.pbr.frag", ::shading::stage::fragment},
            {"./data/effects/terrain.pbr.tesc", ::shading::stage::tesscontrol},
            {"./data/effects/terrain.pbr.tese", ::shading::stage::tessevaluation}
    });

    ::shading::program *p_program_post_processing {new ::shading::program {}};
    api::shading::create_program("processing.post", p_program_post_processing, {
            {"./data/effects/processing.post.vert", ::shading::stage::vertex},
            {"./data/effects/processing.post.frag", ::shading::stage::fragment},
    });

    ::shading::program *p_program_hd_luminance {new ::shading::program {}};
    api::shading::create_program("hdr.luminance.script", p_program_hd_luminance, {
            {"./data/scripts/hdr.luminance.script.comp", ::shading::stage::compute}
    });

    this->config_fog_distance.set_value({0.0f, 512.0f});
    this->config_fog_color.set_value({1.0f, 1.0f, 1.0f});

    auto *&p_camera {api::world::current_camera()};

    float mesh[] {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f
    };

    this->buffer_post_processing.invoke();
    this->buffer_post_processing.stride[1] = 6;

    /* Vertices. */
    this->buffer_post_processing.bind({GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_post_processing.send(sizeof(float) * 12, mesh, GL_STATIC_DRAW);
    this->buffer_post_processing.attach(0, 2);

    /* Texture coordinates. */
    int32_t it {};
    mesh[it++] = 0.0f; mesh[it++] = 0.0f;
    mesh[it++] = 1.0f; mesh[it++] = 0.0f;
    mesh[it++] = 1.0f; mesh[it++] = -1.0f;
    mesh[it++] = 1.0f; mesh[it++] = -1.0f;
    mesh[it++] = 0.0f; mesh[it++] = -1.0f;
    mesh[it++] = 0.0f; mesh[it++] = 0.0f;

    this->buffer_post_processing.bind({GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_post_processing.send(sizeof(float) * 12, mesh, GL_STATIC_DRAW);
    this->buffer_post_processing.attach(1, 2);
    this->buffer_post_processing.revoke();

    /* Link to immediate shape. */
    this->immshape_post_processing.link(&this->buffer_post_processing, p_program_post_processing);

    this->parallel_post_processing.p_program_parallel = p_program_hd_luminance;
    this->parallel_post_processing.dispatch_groups[0] = 32;
    this->parallel_post_processing.dispatch_groups[1] = 32;
    this->parallel_post_processing.invoke();
    this->parallel_post_processing.revoke();

    /* Process chunking buffer. */
    auto &mesh_chunk {api::world::get()->chunk_mesh_data};
    auto &v {mesh_chunk.get_float_list(mesh::type::vertex)};
    auto &t {mesh_chunk.get_float_list(mesh::type::textcoord)};
    auto &c {mesh_chunk.get_float_list(mesh::type::color)};
    auto &i {mesh_chunk.get_uint_list(mesh::type::vertex)};

    this->buffer_chunk.invoke();
    this->buffer_chunk.bind({GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_chunk.send(sizeof(float) * v.size(), v.data(), GL_STATIC_DRAW);
    this->buffer_chunk.attach(0, 3);

    this->buffer_chunk.bind({GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_chunk.send(sizeof(float) * t.size(), t.data(), GL_STATIC_DRAW);
    this->buffer_chunk.attach(1, 2);

    this->buffer_chunk.stride[0] = 0;
    this->buffer_chunk.stride[1] = mesh_chunk.faces;

    this->buffer_chunk.tessellation(4);
    this->buffer_chunk.revoke();
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
    auto &p_camera {api::world::current_camera()};
    this->mat4x4_mvp = p_camera->get_perspective() * p_camera->get_view();

    /* Yes? */
    switch (this->config_post_processing.get_value()) {
        case true: {
            this->process_post_processing();
            break;
        }

        case false: {
            this->process_terrain();
            this->process_environment();
            break;
        }
    }
}

void renderer::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);

    switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            if (sdl_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                this->process_framebuffer(sdl_event.window.data1, sdl_event.window.data2);
            }
            break;
        }

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

    this->wf_chunk_draw_list.push_back(p_chunk);
}

void renderer::refresh() {
    this->update_disabled_chunks = true;
}

void renderer::process_framebuffer(int32_t w, int32_t h) {
    this->framebuffer_post_processing.invoke(0);
    this->framebuffer_post_processing.send_depth({w, h, 0}, {GL_TEXTURE_2D, GL_RGBA32F}, true);
    this->framebuffer_post_processing.revoke();
}