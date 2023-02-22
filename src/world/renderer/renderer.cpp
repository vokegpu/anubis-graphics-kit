#include "renderer.hpp"
#include "world/environment/entity.hpp"
#include "world/environment/object.hpp"
#include "agk.hpp"
#include "util/event.hpp"
#include "world/environment/light.hpp"
#include "asset/shader.hpp"
#include "asset/texture.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

void renderer::process_terrain() {
    glm::mat4 mat4x4_model {};
    glm::mat3 mat3x3_inverse_transpose {};
    glm::vec3 color {1.0f, 1.0f, 1.0f};

    auto p_time_manager {agk::world::sky()};
    ::asset::shader *p_program_pbr {(::asset::shader*) agk::asset::find("gpu/effects.terrain.pbr")};

    p_program_pbr->invoke();
    p_program_pbr->set_uniform_vec2("uFog.uDistance", &agk::app.setting.fog_bounding.get_value()[0]);
    p_program_pbr->set_uniform_vec3("uFog.uColor", &agk::app.setting.fog_color.get_value()[0]);
    p_program_pbr->set_uniform_vec3("uMaterial.uColor", &color[0]);
    p_program_pbr->set_uniform_bool("uMaterial.uMetal", false);
    p_program_pbr->set_uniform_float("uMaterial.uRough", 0.93f);
    p_program_pbr->set_uniform_vec3("uCameraPos", &agk::world::current_camera()->transform.position[0]);
    p_program_pbr->set_uniform_float("uAmbientColor", p_time_manager->ambient_light);
    p_program_pbr->set_uniform_float("uAmbientLuminance", p_time_manager->ambient_luminance);
    p_program_pbr->set_uniform_int("uTerrainHeight", agk::app.setting.chunk_terrain_height.get_value());

    glCullFace(GL_FRONT);
    this->buffer_chunk.invoke();

    glActiveTexture(GL_TEXTURE0);
    /* Height map texture. */

    for (chunk *&p_chunks : this->wf_chunk_draw_list) {
        if (p_chunks == nullptr || !p_chunks->is_processed() || p_chunks->is_dead) {
            continue;
        }

        glBindTexture(GL_TEXTURE_2D, p_chunks->texture);

        mat4x4_model = glm::mat4(1);
        mat4x4_model = glm::translate(mat4x4_model, p_chunks->transform.position);
        mat4x4_model = glm::scale(mat4x4_model, p_chunks->transform.scale);

        mat3x3_inverse_transpose = glm::inverseTranspose(glm::mat3(mat4x4_model));
        p_program_pbr->set_uniform_mat3("uNormalMatrix", &mat3x3_inverse_transpose[0][0]);
        p_program_pbr->set_uniform_mat4("uMatrixModel", &mat4x4_model[0][0]);

        mat4x4_model = this->mat4x4_perspective_view * mat4x4_model;
        p_program_pbr->set_uniform_mat4("uMVP_TescStage", &mat4x4_model[0][0]);
        p_program_pbr->set_uniform_mat4("uMVP_TeseStage", &mat4x4_model[0][0]);

        this->buffer_chunk.draw();
    }

    this->texture_chunk.revoke();
    this->buffer_chunk.revoke();

    glUseProgram(0);
    glCullFace(GL_BACK);
}

void renderer::process_environment() {
    glm::mat4 mat4x4_model {};
    glm::mat3 mat3x3_inverse {};
    glm::mat4 mat4x4_model_view {};

    ::asset::shader *p_program_pbr {(::asset::shader*) agk::asset::find("gpu/effects.material.brdf.pbr")};

    p_program_pbr->invoke();
    p_program_pbr->set_uniform_vec2("uFog.uDistance", &agk::app.setting.fog_bounding.get_value()[0]);
    p_program_pbr->set_uniform_vec3("uFog.uColor", &agk::app.setting.fog_color.get_value()[0]);
    p_program_pbr->set_uniform_float("uAmbientColor", agk::world::sky()->ambient_light);
    p_program_pbr->set_uniform_vec3("uCameraPos", &agk::app.p_curr_camera->transform.position[0]);
    p_program_pbr->set_uniform_mat4("uPerspectiveView", &this->mat4x4_perspective_view[0][0]);

    entity *p_entity {};
    ::asset::model *p_model {};
    object *p_object {};
    light *p_light {};

    enums::material material_type {};
    int32_t light_amount {};
    std::string light_index_tag {};
    float dist_render {agk::app.setting.fog_bounding.get_value().y};

    for (object *&p_world_feature : this->wf_env_draw_list) {
        if (p_world_feature == nullptr || p_world_feature->is_dead) {
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

            case enums::type::light: {
                p_light = (light*) p_world_feature;
                p_light->index = light_amount++;
                p_model = p_light->p_model;
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
        mat4x4_model = glm::translate(mat4x4_model, p_object->transform.position);
        mat4x4_model = glm::rotate(mat4x4_model, p_object->transform.rotation.x, {1, 0, 0});
        mat4x4_model = glm::rotate(mat4x4_model, p_object->transform.rotation.y, {0, 1, 0});
        mat4x4_model = glm::rotate(mat4x4_model, p_object->transform.rotation.z, {0, 0, 1});
        mat4x4_model = glm::scale(mat4x4_model, p_object->transform.scale);

        if (((glm::distance(agk::app.p_curr_camera->transform.position, p_object->transform.position) > dist_render || !agk::app.p_curr_camera->viewing(mat4x4_model, p_world_feature->transform.scale, p_model->axis_aligned_bounding_box)) && !p_model->buffer.instancing_rendering)) {
            continue;
        }

        p_program_pbr->set_uniform_bool("uInstanced", p_model->buffer.instancing_rendering);
        switch (p_model->buffer.instancing_rendering) {
            case true: {
                p_program_pbr->set_uniform_mat4("uPerspectiveViewMatrix", &this->mat4x4_perspective_view[0][0]);

                material_type = p_object->p_material->get_type();
                p_program_pbr->set_uniform_bool("uMaterial.uMetal", material_type == enums::material::metal);
                p_program_pbr->set_uniform_float("uMaterial.uRough", p_object->p_material->get_rough());
                p_program_pbr->set_uniform_vec3("uMaterial.uColor", &p_object->p_material->get_color()[0]);

                p_model->buffer.invoke();
                p_model->buffer.draw();
                p_model->buffer.revoke();
                break;
            }

            case false: {
                mat3x3_inverse = glm::inverseTranspose(glm::mat3(mat4x4_model));
                p_program_pbr->set_uniform_mat3("uNormalMatrix", &mat3x3_inverse[0][0]);
                p_program_pbr->set_uniform_mat4("uModelMatrix", &mat4x4_model[0][0]);

                mat4x4_model = this->mat4x4_perspective_view * mat4x4_model;
                p_program_pbr->set_uniform_mat4("uMVP", &mat4x4_model[0][0]);

                material_type = p_object->p_material->get_type();
                p_program_pbr->set_uniform_bool("uMaterial.uMetal", material_type == enums::material::metal);
                p_program_pbr->set_uniform_float("uMaterial.uRough", p_object->p_material->get_rough());
                p_program_pbr->set_uniform_vec3("uMaterial.uColor", &p_object->p_material->get_color()[0]);

                p_model->buffer.invoke();
                p_model->buffer.draw();
                p_model->buffer.revoke();
                break;
            }
        }
    }

    if (this->loaded_light_size != light_amount) {
        this->loaded_light_size = light_amount;
        p_program_pbr->set_uniform_int("uLightAmount", light_amount);

        p_program_pbr = (::asset::shader*) agk::asset::find("gpu/effects.terrain.pbr");
        p_program_pbr->invoke();
        p_program_pbr->set_uniform_int("uLightAmount", light_amount);
    };

    glUseProgram(0);
}

void renderer::process_post_processing() {
    /* Invoke framebuffer and start collect screen buffers. */
    this->framebuffer_post_processing.invoke(0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->process_terrain();
    this->process_environment();
    this->process_sky();

    /* Revoke all buffers from frame. */
    this->framebuffer_post_processing.revoke(GL_COLOR_BUFFER_BIT);

    /* Draw the current frame buffer. */
    float ave_lum {};

    if (agk::app.setting.enable_hdr.get_value() && util::reset_when(this->timing_hdr_cycle, 16)) {
        int32_t size {agk::app.screen_width * agk::app.screen_height};

        this->parallel_post_processing.memory_barrier = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
        this->parallel_post_processing.dispatch_groups[0] = 32;
        this->parallel_post_processing.dispatch_groups[1] = 32;
        this->parallel_post_processing.dimension[0] = agk::app.screen_width;
        this->parallel_post_processing.dimension[1] = agk::app.screen_height;

        /* Pass current framebuffer to HDR luminance texture.  */
        this->parallel_post_processing.invoke();
        this->texture_post_processing.invoke(0, {GL_TEXTURE_2D, GL_FLOAT});
        this->texture_post_processing[0].id = this->framebuffer_post_processing[0].id_texture;
        this->texture_post_processing[0].w = agk::app.screen_width;
        this->texture_post_processing[0].h = agk::app.screen_height;
        this->texture_post_processing[0].channel = GL_RGBA;
        this->texture_post_processing[0].format = GL_RGBA32F;
        this->parallel_post_processing.attach(0, this->texture_post_processing[0], GL_READ_ONLY);

        this->texture_post_processing.invoke(1, {GL_TEXTURE_2D, GL_FLOAT});
        this->texture_post_processing.send<float>({1, 1, 1}, nullptr, {GL_R32F, GL_RED});
        this->parallel_post_processing.attach(1, this->texture_post_processing[1], GL_WRITE_ONLY);
        this->parallel_post_processing.dispatch();

        /* Read the HDR parallel script with the luminosity average calculated. */
        this->texture_post_processing.invoke(1, {GL_TEXTURE_2D, GL_FLOAT});

        float luminance {};
        this->texture_post_processing.get<float>(&luminance);
        ave_lum = expf(luminance / static_cast<float>(size));

        this->texture_post_processing.revoke();
        this->parallel_post_processing.revoke();
    }

    glCullFace(GL_FRONT);

    /* Bind the global texture framebuffer (first pass after scene rendering) to apply the post-processing filters. */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->framebuffer_post_processing[0].id_texture);

    this->immshape_post_processing.invoke();
    this->immshape_post_processing.p_program->set_uniform_float("uHDR.uAverageLuminance", ave_lum);
    this->immshape_post_processing.p_program->set_uniform_float("uHDR.uExposure", agk::app.setting.hdr_exposure.get_value());
    this->immshape_post_processing.p_program->set_uniform_bool("uHDR.uEnabled", agk::app.setting.enable_hdr.get_value());

    this->immshape_post_processing.p_program->set_uniform_bool("uMotionBlur.uEnabled", agk::app.setting.enable_motion_blur.get_value());
    if (agk::app.setting.enable_motion_blur.get_value()) {
        this->mat4x4_inverse_perspective_view = glm::inverse(this->mat4x4_perspective_view);
        this->immshape_post_processing.p_program->set_uniform_mat4("uMotionBlur.uInversePerspectiveView", &this->mat4x4_inverse_perspective_view[0][0]);
        this->immshape_post_processing.p_program->set_uniform_mat4("uMotionBlur.uPreviousPerspectiveView", &this->mat4x4_previous_perspective_view[0][0]);
        this->mat4x4_previous_perspective_view = this->mat4x4_perspective_view;

        auto p_camera {agk::app.p_curr_camera};
        float yaw {p_camera->transform.rotation.x};
        float pitch {p_camera->transform.rotation.y};

        /* Check if there is no movement in camera. */
        this->camera_motion_delta.x = yaw - this->camera_motion_delta.x;
        this->camera_motion_delta.y = pitch - this->camera_motion_delta.y;
        float acc {(this->camera_motion_delta.x * this->camera_motion_delta.x + this->camera_motion_delta.y * this->camera_motion_delta.y)};
        this->immshape_post_processing.p_program->set_uniform_bool("uMotionBlur.uCameraRotated", acc != 0.0f);
        this->camera_motion_delta.x = yaw;
        this->camera_motion_delta.y = pitch;

        this->immshape_post_processing.p_program->set_uniform_float("uMotionBlur.uIntensity", agk::app.setting.motion_blur_intensity.get_value());
    }

    this->immshape_post_processing.draw({0, 0, agk::app.screen_width, agk::app.screen_height}, {.0f, 0.0f, 1.0f, 1.0f});
    this->immshape_post_processing.revoke();

    glCullFace(GL_BACK);
}

void renderer::process_editor() {
    ::asset::shader *p_debug_program {(::asset::shader*) agk::asset::find("gpu/effects.coordinate.debug")};

    glm::mat4 mat4x4_model {1.0f};
    mat4x4_model = glm::scale(mat4x4_model, {20, 20, 20});
    mat4x4_model = this->mat4x4_perspective_view * mat4x4_model;

    p_debug_program->invoke();
    p_debug_program->set_uniform_mat4("uMVP", &mat4x4_model[0][0]);
    this->buffer_coordinate_debug.invoke();

    glDisable(GL_DEPTH_TEST);
    this->buffer_coordinate_debug.draw();
    glEnable(GL_DEPTH_TEST);

    this->buffer_coordinate_debug.revoke();
    p_debug_program->revoke();
}

void renderer::on_create() {
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
    this->buffer_post_processing.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_post_processing.send<float>(sizeof(mesh), mesh, GL_STATIC_DRAW);
    this->buffer_post_processing.attach(0, 2);
    this->buffer_post_processing.revoke();

    /* Link to immediate shape. */
    this->immshape_post_processing.link(&this->buffer_post_processing, (::asset::shader*) agk::asset::find("gpu/effects.processing.post"));

    this->parallel_post_processing.p_program = (::asset::shader*) agk::asset::find("gpu/scripts.hdr.luminance.post");
    this->parallel_post_processing.dispatch_groups[0] = 32;
    this->parallel_post_processing.dispatch_groups[1] = 32;

    /* Process chunking buffer. */
    auto &mesh_chunk {agk::world::get()->chunk_mesh_data};
    auto &v {mesh_chunk.get_float_list(mesh::type::vertex)};
    auto &t {mesh_chunk.get_float_list(mesh::type::textcoord)};
    auto &c {mesh_chunk.get_float_list(mesh::type::color)};
    auto &i {mesh_chunk.get_uint_list(mesh::type::vertex)};

    this->buffer_chunk.invoke();
    this->buffer_chunk.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_chunk.send<float>(sizeof(float) * v.size(), v.data(), GL_STATIC_DRAW);
    this->buffer_chunk.attach(0, 3);

    this->buffer_chunk.bind(1, {GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_chunk.send<float>(sizeof(float) * t.size(), t.data(), GL_STATIC_DRAW);
    this->buffer_chunk.attach(1, 2);

    this->buffer_chunk.stride[0] = 0;
    this->buffer_chunk.stride[1] = mesh_chunk.faces;

    this->buffer_chunk.tessellation(4);
    this->buffer_chunk.revoke();

    float len {1.0f};

    /* Prepare the coordinate debug GPU buffer(s). */
    float coordinates[] {
        0.0f, 0.0f, 0.0f, /**/ 1.0f, 0.0f, 0.0f,
        len, 0.0f, 0.0f,  /**/ 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f, /**/ 0.0f, 1.0f, 0.0f,
        0.0f, len, 0.0f,  /**/ 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, /**/ 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, len,  /**/ 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.0f,  /**/ 1.0f, 1.0f, 1.0f,
        -len, 0.0f, 0.0f,  /**/ 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f,  /**/ 1.0f, 1.0f, 1.0f,
        0.0f, -len, 0.0f,  /**/ 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f,  /**/ 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, -len,  /**/ 1.0f, 1.0f, 1.0f
    };

    this->buffer_coordinate_debug.stride[1] = 12;
    this->buffer_coordinate_debug.primitive[0] = GL_LINES;

    this->buffer_coordinate_debug.invoke();
    this->buffer_coordinate_debug.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_coordinate_debug.send<float>(sizeof(coordinates), coordinates, GL_STATIC_DRAW);
    this->buffer_coordinate_debug.attach(0, 3, {sizeof(float) * 6, 0});
    this->buffer_coordinate_debug.attach(1, 3, {sizeof(float) * 6, sizeof(float) * 3});
    this->buffer_coordinate_debug.revoke();

    auto *p_terrain_atlas {(asset::texture<uint8_t>*) agk::asset::find("texture/terrain.atlas")};
    p_terrain_atlas->add("stone", {0.0f, 0.0f, 1.0f, 1.0f});

    auto *p_program {(asset::shader*) agk::asset::find("gpu/effects.terrain.pbr")};
    p_program->invoke();

    /* Attach/store atlas texture coordinates. */
    glActiveTexture(GL_TEXTURE1);
    p_terrain_atlas->invoke();
    p_terrain_atlas->attach("uAtlas", p_program);
    p_program->revoke();
}

void renderer::on_destroy() {

}

void renderer::on_render() {
    if (this->update_disabled_chunks) {
        this->update_disabled_chunks = false;
        this->wf_chunk_draw_list = agk::world::get()->loaded_chunk_list;
    }

    /* Prepare matrices to render the world. */
    auto &p_camera {agk::world::current_camera()};
    this->mat4x4_perspective = p_camera->get_perspective();
    this->mat4x4_view = p_camera->get_view();
    this->mat4x4_perspective_view = this->mat4x4_perspective * this->mat4x4_view;

    /* Yes? */
    switch (agk::app.setting.enable_post_processing.get_value()) {
        case true: {
            this->process_post_processing();
            this->process_editor();
            break;
        }

        case false: {
            this->process_terrain();
            this->process_environment();
            this->process_editor();
            this->process_sky();
            break;
        }
    }
}

void renderer::on_event(SDL_Event &sdl_event) {
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
    auto &world {agk::world::get()};

    auto *p_to_remove {static_cast<bool*>(sdl_event.user.data1)};
    auto *p_wf_id {static_cast<int32_t*>(sdl_event.user.data2)};

    object *&p_world_feature {world->find_env_wf(*p_wf_id)};
    if (*p_to_remove) {
        this->wf_env_draw_list.clear();
        if (p_world_feature != nullptr) p_world_feature->set_visible(enums::state::disable, false);

        for (object *&p_world_features : world->wf_list) {
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
    auto *&p_world {agk::world::get()};
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

void renderer::process_sky() {
    // @TODO skybox rendering.
}