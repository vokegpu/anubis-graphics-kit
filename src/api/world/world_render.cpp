#include "world_render.hpp"
#include "api/api.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "api/util/env.hpp"

void world_render::on_create() {
    feature::on_create();

    api::shading::createprogram("object-model", this->object_model_shading, {
            {"./effects/ObjectModel.vsh", shading::stage::vertex},
            {"./effects/ObjectModel.fsh", shading::stage::fragment}
    });

    api::shading::createprogram("entity-model", this->entity_model_shading, {
            {"./effects/EntityModel.vsh", shading::stage::vertex},
            {"./effects/EntityModel.fsh", shading::stage::fragment}
    });
}

void world_render::on_destroy() {
    feature::on_destroy();
}

void world_render::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);

    switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            switch (sdl_event.window.event) {
                case SDL_WINDOWEVENT_SIZE_CHANGED: {
                    api::app.screen_width = sdl_event.window.data1;
                    api::app.screen_height = sdl_event.window.data2;
                    this->update_perspective_matrix();
                    break;
                }
            }

            break;
        }
    }
}

void world_render::on_update() {
    feature::on_update();
}

void world_render::on_render() {
    feature::on_render();

    if (this->loaded_model_list.empty()) {
        return;
    }

    glm::mat4 model {};
    glm::mat3 normal {};
    glm::mat4 mvp {};

    buffer_builder* current_buffer_builder {};
    auto loaded_model_list_size {this->loaded_model_list.size()};
    glm::mat4 camera_view {api::app.world_camera3d.get_matrix_camera_view()};

    glUseProgram(this->object_model_shading.id);
    this->object_model_shading.set_uniform_mat4("MatrixPerspective", &this->matrix_perspective[0][0]);
    this->object_model_shading.set_uniform_float("Material.Rough", 1.0f);

    int32_t light_iterations_id {};
    std::string id {};
    material::light* light_material {};
    material::solid* solid_material {};

    for (object* &objects : api::app.world_client.loaded_object_list) {
        switch (objects->material->composition) {
            case material::composition::light: {
                light_material = (material::light*) objects->material;
                id = std::to_string(light_iterations_id);

                this->object_model_shading.set_uniform_vec3("Light[" + id + "].Position", &(glm::mat3(camera_view) * objects->position)[0]);
                this->object_model_shading.set_uniform_bool("Light[" + id + "].Incoming", light_material->incoming);
                this->object_model_shading.set_uniform_vec3("Light[" + id + "].Intensity", &light_material->intensity[0]);
                light_iterations_id++;
                break;
            }

            default: {
                if (objects->model_id > loaded_model_list_size || objects->model_id < 0) {
                    break;
                }

                current_buffer_builder = this->loaded_model_list.at(objects->model_id);
                if (current_buffer_builder == nullptr) {
                    break;
                }

                solid_material = (material::solid*) objects->material;

                model = glm::mat4(1.0f);
                model = glm::translate(model, objects->position);
                model = glm::rotate(model, objects->rotation.x, glm::vec3(1, 0, 0));
                model = glm::rotate(model, objects->rotation.y, glm::vec3(0, 1, 0));
                model = glm::rotate(model, objects->rotation.z, glm::vec3(0, 0, 1));
                model = glm::scale(model, objects->scale);
                model = camera_view * model;
                mvp = this->matrix_perspective * model;

                this->object_model_shading.set_uniform_mat3("NormalMatrix", &glm::mat3(model)[0][0]);
                this->object_model_shading.set_uniform_mat4("ModelViewMatrix", &model[0][0]);
                this->object_model_shading.set_uniform_mat4("MVP", &mvp[0][0]);
                this->object_model_shading.set_uniform_vec3("Material.Color", solid_material->color);
                this->object_model_shading.set_uniform_bool("Material.Metal", objects->material->composition == material::composition::metal);

                current_buffer_builder->on_render();
                break;
            }
        }
    }

    if (this->spot_lights_on_world != light_iterations_id) {
        this->spot_lights_on_world = light_iterations_id;
        this->object_model_shading.set_uniform_int("LoadedLightsIndex", light_iterations_id);
    }

    glUseProgram(0);
}

buffer_builder *world_render::gen_model(const char* tag) {
    auto model {new buffer_builder {tag}};
    api::gc::create(model);

    this->loaded_model_list.push_back(model);
    model->id = this->loaded_model_list.size() - 1;
    this->registered_models_map[std::string(model->tag)] = model->id;

    return model;
}

void world_render::update_perspective_matrix() {
    float aspect_ration {static_cast<float>(api::app.screen_width) / static_cast<float>(api::app.screen_height)};
    this->matrix_perspective = glm::perspective(glm::radians(api::app.world_camera3d.field_of_view), aspect_ration, 0.1f, 1000.0f);
}

buffer_builder* world_render::get_model_by_tag(std::string_view tag) {
    return this->loaded_model_list[this->registered_models_map[tag.data()]];
}

int32_t world_render::get_model_id_by_tag(std::string_view tag) {
    return this->registered_models_map[tag.data()];
}