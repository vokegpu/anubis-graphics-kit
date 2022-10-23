#include "world_render.hpp"
#include "api/api.hpp"

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
}

void world_render::on_update() {
    feature::on_update();
}

void world_render::on_render() {
    feature::on_render();
}
