#include "api/world/renderer/renderer.hpp"
#include "api/world/environment/entity.hpp"
#include "api/world/model/model.hpp"
#include "api/world/environment/object.hpp"
#include "api/api.hpp"
#include "api/event/event.hpp"
#include "api/world/environment/light.hpp"
#include "api/asset/impl/shader.hpp"
#include "api/asset/impl/texture.hpp"
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
    p_model->tag = tag.data();

    this->loaded_model_list.push_back(p_model);
    this->model_register_map[tag.data()] = p_model->id;

    auto &buffer {p_model->buffer};
    buffer.invoke();
    buffer.stride[0] = 0;
    buffer.instancing_rendering = mesh_data.instanced;

    if (mesh_data.contains(mesh::type::vertex)) {
        f_list = mesh_data.get_float_list(mesh::type::vertex);
        buffer.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
        buffer.send<float>(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffer.attach(0, 3);
        buffer.stride[1] = f_list.size() / 3;

        glm::vec3 min {glm::vec3(std::numeric_limits<float>::max())};
        glm::vec3 max {glm::vec3(std::numeric_limits<float>::min())};

        for (int32_t it {}; it < buffer.stride[1]; it++) {
            const float *p_vec {&f_list.at(it * 3)};
            min.x = std::min(min.x, p_vec[0]);
            min.y = std::min(min.y, p_vec[1]);
            min.z = std::min(min.z, p_vec[2]);

            max.x = std::max(max.x, p_vec[0]);
            max.y = std::max(max.y, p_vec[1]);
            max.z = std::max(max.z, p_vec[2]);
        }

        p_model->axis_aligned_bounding_box = util::aabb(min, max);
    }

    if (mesh_data.contains(mesh::type::textcoord)) {
        f_list = mesh_data.get_float_list(mesh::type::textcoord);
        buffer.bind(1, {GL_ARRAY_BUFFER, GL_FLOAT});
        buffer.send<float>(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffer.attach(1, 2);
    }

    if (mesh_data.contains(mesh::type::normal)) {
        f_list = mesh_data.get_float_list(mesh::type::normal);
        buffer.bind(2, {GL_ARRAY_BUFFER, GL_FLOAT});
        buffer.send<float>(sizeof(float) * f_list.size(), f_list.data(), GL_STATIC_DRAW);
        buffer.attach(2, 3);
    }

    if (mesh_data.contains(mesh::type::vertex, true)) {
        i_list = mesh_data.get_uint_list(mesh::type::vertex);
        buffer.bind(3, {GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT});
        buffer.send<uint32_t>(sizeof(uint32_t) * i_list.size(), i_list.data(), GL_STATIC_DRAW);
        buffer.stride[0] = mesh_data.faces;
        buffer.stride[1] = 0;
    }

    if (mesh_data.instanced) {
        buffer.bind(4, {GL_ARRAY_BUFFER, GL_FLOAT});
        buffer.send<float>(api::app.setting.maximum_buffers.get_value() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        buffer.attach(3, 4, {sizeof(glm::mat4), 0});
        buffer.attach(4, 4, {sizeof(glm::mat4), sizeof(glm::vec4)});
        buffer.attach(5, 4, {sizeof(glm::mat4), sizeof(glm::vec4) * 2});
        buffer.attach(6, 4, {sizeof(glm::mat4), sizeof(glm::vec4) * 3});

        buffer.divisor(3, 1);
        buffer.divisor(4, 1);
        buffer.divisor(5, 1);
        buffer.divisor(6, 1);
    }

    buffer.revoke();
    return p_model;
}

bool renderer::find(std::string_view tag, model *&p_model) {
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
    glm::mat4 mat4x4_model {};
    glm::mat3 mat3x3_inverse_transpose {};
    glm::vec3 color {1.0f, 1.0f, 1.0f};

    auto p_time_manager {api::world::time_manager()};
    ::asset::shader *p_program_pbr {(::asset::shader*) api::asset::find("gpu/effects.terrain.pbr")};

    p_program_pbr->invoke();
    p_program_pbr->set_uniform_vec2("uFog.uDistance", &api::app.setting.fog_bounding.get_value()[0]);
    p_program_pbr->set_uniform_vec3("uFog.uColor", &api::app.setting.fog_color.get_value()[0]);
    p_program_pbr->set_uniform_vec3("uMaterial.uColor", &color[0]);
    p_program_pbr->set_uniform_bool("uMaterial.uMetal", false);
    p_program_pbr->set_uniform_float("uMaterial.uRough", 0.93f);
    p_program_pbr->set_uniform_vec3("uCameraPos", &api::world::current_camera()->position[0]);
    p_program_pbr->set_uniform_float("uAmbientColor", p_time_manager->ambient_light);
    p_program_pbr->set_uniform_float("uAmbientLuminance", p_time_manager->ambient_luminance);
    p_program_pbr->set_uniform_int("uTerrainHeight", api::app.setting.chunk_terrain_height.get_value());

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
        mat4x4_model = glm::translate(mat4x4_model, p_chunks->position);
        mat4x4_model = glm::scale(mat4x4_model, p_chunks->scale);

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

    ::asset::shader *p_program_pbr {(::asset::shader*) api::asset::find("gpu/effects.material.brdf.pbr")};

    p_program_pbr->invoke();
    p_program_pbr->set_uniform_vec2("uFog.uDistance", &api::app.setting.fog_bounding.get_value()[0]);
    p_program_pbr->set_uniform_vec3("uFog.uColor", &api::app.setting.fog_color.get_value()[0]);
    p_program_pbr->set_uniform_float("uAmbientColor", api::world::time_manager()->ambient_light);
    p_program_pbr->set_uniform_vec3("uCameraPos", &api::app.p_curr_camera->position[0]);
    p_program_pbr->set_uniform_mat4("uPerspectiveView", &this->mat4x4_perspective_view[0][0]);

    entity *p_entity {};
    model *p_model {};
    object *p_object {};
    light *p_light {};

    enums::material material_type {};
    int32_t light_amount {};
    std::string light_index_tag {};
    float dist_render {api::app.setting.fog_bounding.get_value().y};

    for (world_feature *&p_world_feature : this->wf_env_draw_list) {
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
        mat4x4_model = glm::translate(mat4x4_model, p_object->position);
        mat4x4_model = glm::rotate(mat4x4_model, p_object->rotation.x, {1, 0, 0});
        mat4x4_model = glm::rotate(mat4x4_model, p_object->rotation.y, {0, 1, 0});
        mat4x4_model = glm::rotate(mat4x4_model, p_object->rotation.z, {0, 0, 1});
        mat4x4_model = glm::scale(mat4x4_model, p_object->scale);

        if (((glm::distance(api::app.p_curr_camera->position, p_object->position) > dist_render || !api::app.p_curr_camera->viewing(mat4x4_model, p_world_feature->scale, p_model->axis_aligned_bounding_box)) && !p_model->buffer.instancing_rendering)) {
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

        p_program_pbr = (::asset::shader*) api::asset::find("gpu/effects.terrain.pbr");
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

    if (api::app.setting.enable_hdr.get_value() && util::reset_when(this->timing_hdr_cycle, 16)) {
        int32_t size {api::app.screen_width * api::app.screen_height};

        this->parallel_post_processing.memory_barrier = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
        this->parallel_post_processing.dispatch_groups[0] = 32;
        this->parallel_post_processing.dispatch_groups[1] = 32;
        this->parallel_post_processing.dimension[0] = api::app.screen_width;
        this->parallel_post_processing.dimension[1] = api::app.screen_height;

        /* Pass current framebuffer to HDR luminance texture.  */
        this->parallel_post_processing.invoke();
        this->texture_post_processing.invoke(0, {GL_TEXTURE_2D, GL_FLOAT});
        this->texture_post_processing[0].id = this->framebuffer_post_processing[0].id_texture;
        this->texture_post_processing[0].w = api::app.screen_width;
        this->texture_post_processing[0].h = api::app.screen_height;
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
    this->immshape_post_processing.p_program->set_uniform_float("uHDR.uExposure", api::app.setting.hdr_exposure.get_value());
    this->immshape_post_processing.p_program->set_uniform_bool("uHDR.uEnabled", api::app.setting.enable_hdr.get_value());

    this->immshape_post_processing.p_program->set_uniform_bool("uMotionBlur.uEnabled", api::app.setting.enable_motion_blur.get_value());
    if (api::app.setting.enable_motion_blur.get_value()) {
        this->mat4x4_inverse_perspective_view = glm::inverse(this->mat4x4_perspective_view);
        this->immshape_post_processing.p_program->set_uniform_mat4("uMotionBlur.uInversePerspectiveView", &this->mat4x4_inverse_perspective_view[0][0]);
        this->immshape_post_processing.p_program->set_uniform_mat4("uMotionBlur.uPreviousPerspectiveView", &this->mat4x4_previous_perspective_view[0][0]);
        this->mat4x4_previous_perspective_view = this->mat4x4_perspective_view;

        auto p_camera {api::app.p_curr_camera};
        float yaw {p_camera->rotation.x};
        float pitch {p_camera->rotation.y};

        /* Check if there is no movement in camera. */
        this->camera_motion_delta.x = yaw - this->camera_motion_delta.x;
        this->camera_motion_delta.y = pitch - this->camera_motion_delta.y;
        float acc {(this->camera_motion_delta.x * this->camera_motion_delta.x + this->camera_motion_delta.y * this->camera_motion_delta.y)};
        this->immshape_post_processing.p_program->set_uniform_bool("uMotionBlur.uCameraRotated", acc != 0.0f);
        this->camera_motion_delta.x = yaw;
        this->camera_motion_delta.y = pitch;

        this->immshape_post_processing.p_program->set_uniform_float("uMotionBlur.uIntensity", api::app.setting.motion_blur_intensity.get_value());
    }

    this->immshape_post_processing.draw({0, 0, api::app.screen_width, api::app.screen_height}, {.0f, 0.0f, 1.0f, 1.0f});
    this->immshape_post_processing.revoke();

    glCullFace(GL_BACK);
}

void renderer::process_editor() {
    ::asset::shader *p_debug_program {(::asset::shader*) api::asset::find("gpu/effects.coordinate.debug")};

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
    feature::on_create();

    this->instance_mat4x4.reserve(sizeof(glm::mat4) * api::app.setting.maximum_buffers.get_value());
    this->instance_mat4x4.reserve(sizeof(glm::mat3) * api::app.setting.maximum_buffers.get_value());

    util::log("Reserved " + std::to_string((sizeof(glm::mat4) + sizeof(glm::mat3)) * api::app.setting.maximum_buffers.get_value()) + " memory.");

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
    this->immshape_post_processing.link(&this->buffer_post_processing, (::asset::shader*) api::asset::find("gpu/effects.processing.post"));

    this->parallel_post_processing.p_program = (::asset::shader*) api::asset::find("gpu/scripts.hdr.luminance.post");
    this->parallel_post_processing.dispatch_groups[0] = 32;
    this->parallel_post_processing.dispatch_groups[1] = 32;

    /* Process chunking buffer. */
    auto &mesh_chunk {api::world::get()->chunk_mesh_data};
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

    auto *p_terrain_atlas {(asset::texture<uint8_t>*) api::asset::find("texture/terrain.atlas")};
    p_terrain_atlas->add("stone", {0.0f, 0.0f, 1.0f, 1.0f});

    auto *p_program {(asset::shader*) api::asset::find("gpu/effects.terrain.pbr")};
    p_program->invoke();

    /* Attach/store atlas texture coordinates. */
    glActiveTexture(GL_TEXTURE1);
    p_terrain_atlas->invoke();
    p_terrain_atlas->attach("uAtlas", p_program);
    p_program->revoke();
}

void renderer::on_destroy() {
    feature::on_destroy();
}

void renderer::on_render() {
    feature::on_render();

    if (this->update_disabled_chunks) {
        this->update_disabled_chunks = false;
        this->wf_chunk_draw_list = api::world::get()->loaded_chunk_list;
    }

    /* Prepare matrices to render the world. */
    auto &p_camera {api::world::current_camera()};
    this->mat4x4_perspective = p_camera->get_perspective();
    this->mat4x4_view = p_camera->get_view();
    this->mat4x4_perspective_view = this->mat4x4_perspective * this->mat4x4_view;

    /* Yes? */
    switch (api::app.setting.enable_post_processing.get_value()) {
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

    if (util::reset_when(this->timing_instances, 1000)) {
        this->process_instances();
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

    world_feature *&p_world_feature {world->find_env_wf(*p_wf_id)};
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

void renderer::process_sky() {
    // @TODO skybox rendering.
}

::mesh::data &renderer::interact_with_instanced_mesh(int32_t id) {
    auto &mesh {this->instanced_mesh_map[id]};
    mesh.instanced = true;
    return mesh;
}

int32_t renderer::add_mesh_instance(int32_t id, uint32_t key, buffering &buffer, const glm::mat4 &matrix) {
    // @TODO instancing rendering mesh operations.

    auto &mesh {this->instanced_mesh_map[id]};
    auto &data {mesh.get_instance_list()};

    int32_t size {(int32_t) (sizeof(glm::mat4) * data.size())};
    size = size < 0 ? 0 : size;
    mesh.append_instanced(matrix);

    buffer.bind(key, {GL_ARRAY_BUFFER, GL_FLOAT});
    buffer.edit<float>(size, sizeof(glm::mat4), &matrix[0][0]);
    buffer.unbind();

    util::log(std::to_string(size) + " " + std::to_string(data.size()));
    buffer.stride[2] = data.size();

    return -1;
}

int32_t renderer::remove_mesh_instance(int32_t id, int32_t instance) {
    auto &mesh {this->instanced_mesh_map[id]};
    mesh.remove_instance(instance);
    return 0;
}

void renderer::process_instances() {
    if (!this->instanced_node_list.empty() && false) {
        model *p_model {};
        int64_t amount_instances_3x3 {};
        int64_t amount_instances_4x4 {};
        std::vector<::mesh::instance> instance_list {};
        this->instance_mat4x4.clear();

        for (node &nodes : this->instanced_node_list) {
            if (!this->find(nodes.model, p_model)) {
                continue;
            }

            ::mesh::data &mesh {this->instanced_mesh_map[nodes.parent]};
            amount_instances_4x4 = 0;
            amount_instances_3x3 = 0;
            instance_list = mesh.get_instance_list();

            for (mesh::instance &instances : instance_list) {
                float *p_3x3 {&glm::inverseTranspose(glm::mat3(instances.mat4x4))[0][0]};
                this->instance_mat3x3.insert(this->instance_mat3x3.begin() + amount_instances_3x3, &p_3x3[0], &p_3x3[9]);

                float *p_4x4 {&instances.mat4x4[0][0]};
                this->instance_mat4x4.insert(this->instance_mat4x4.begin() + amount_instances_4x4, &p_4x4[0], &p_4x4[16]);

                amount_instances_3x3 += 9;
                amount_instances_4x4 += 16;
            }

            // Specify how many iterations we need to render.
            p_model->buffer.stride[2] = static_cast<int32_t>(instance_list.size());

            /* Replace buffer data without re-allocate. */
            p_model->buffer.bind(4, {GL_ARRAY_BUFFER, GL_FLOAT});
            p_model->buffer.edit<float>(0, sizeof(float) * amount_instances_4x4, this->instance_mat4x4.data());
            p_model->buffer.unbind();

            //p_model->buffer.bind(5, {GL_ARRAY_BUFFER, GL_FLOAT});
            //p_model->buffer.edit<float>(0, sizeof(float) * amount_instances_3x3, this->instance_mat3x3.data());
            //p_model->buffer.unbind();

            util::log(std::to_string(p_model->buffer.stride[2]));
        }

        this->instanced_node_list.clear();
    }
}
