#include "world_render.hpp"
#include "api/api.hpp"
#include <glm/gtc/matrix_transform.hpp>

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

    this->loaded_model_list.push_back(new buffer_builder {});
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
                    this->matrix_perspective = glm::();
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
    glUseProgram(this->object_model_shading.id);

    glm::mat4 model {};
    glm::mat3 normal {};

    for (object* &objects : api::app.world_client.loaded_object_list) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, objects->position);
        normal = glm::mat3(model);
        normal = glm::transpose(normal);

        this->object_model_shading.set_uniform_mat3("MatrixNormal", &normal[0][0]);
    }

    glUseProgram(0);
}
