#include "starter.hpp"
#include "api/gpu/buffering.hpp"
#include "api/util/env.hpp"
#include "api/api.hpp"
#include "api/world/environment/object.hpp"
#include "api/world/environment/light.hpp"

void client::scenes::starter::on_create() {
    model *p_model_dino {api::world::create("Dinossaur", "./data/models/Dinossaur.stl", mesh::format::stl)};
    material *p_material {new material(enums::material::dialetric)};

    for (int i {}; i < 20; i++) {
        this->p_object_dino = new object(p_model_dino);
        p_material->set_color({1.0f, 215.0f / 255.0f, 0.0f});
        this->p_object_dino->p_material = p_material;
        this->p_object_dino->position.x = rand() % 100;
        this->p_object_dino->position.z = rand() % 100;
        this->p_object_dino->scale = {0.2f, 0.2f, 0.2f};

        api::world::create(this->p_object_dino);
    }

    this->p_light_spot = new light(nullptr);
    this->p_light_spot->intensity = {50, 50, 50};
    this->p_light_spot->position = {0, 15, 0};
    this->p_light_spot->update();

    api::world::create(this->p_light_spot);
    api::world::current_player()->speed_base = 0.9540f;

    this->p_camera_manager->set_editor_enabled(true);
    this->p_camera_manager->set_movement_enabled(true);
    this->p_camera_manager->set_rotation_enabled(true);
    this->p_camera_manager->bind_editor_rotate.set_value("mouse-2");

    this->f_render.load("./data/fonts/impact.ttf", 18);

    ekg::gl_version = "#version 450 core";
    ekg::init(api::app.p_sdl_window, "./data/fonts/JetBrainsMono-Bold.ttf");

    auto frame = ekg::frame("Hello", {20, 20}, {200, 200});
    frame->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::right);
    frame->set_drag(ekg::dock::top);

    ekg::label("Welcome to AGK!", ekg::dock::top | ekg::dock::left);
    ekg::label("Light Intensity:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_light_intensity = ekg::slider("LightIntensity", 50.0f, 0.0f, 255.0f, ekg::dock::top | ekg::dock::left);
    this->p_slider_light_intensity->set_precision(2);

    ekg::label("Base Speed:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_base_speed = ekg::slider("BaseSpeed", 0.9f, 0.1f, 10.0f, ekg::dock::top | ekg::dock::left);
    this->p_slider_base_speed->set_precision(4);

    ekg::label("Chunk Range:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_range = ekg::slider("ChunkRange", 3, 1, 8, ekg::dock::top | ekg::dock::left);
    this->p_slider_range->set_precision(1);

    ekg::label("Noise X:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_noise_control_x = ekg::slider("NoiseX", 2.0f, 0.0f, 100.0f, ekg::dock::top | ekg::dock::left);
    this->p_slider_noise_control_x->set_precision(2);

    ekg::label("Noise Y:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_noise_control_y = ekg::slider("NoiseY", 2.0f, 0.0f, 100.0f, ekg::dock::top | ekg::dock::left);
    this->p_slider_noise_control_y->set_precision(2);

    ekg::label("Noise Offset:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_noise_control_offset = ekg::slider("NoiseOffset", 0.5f, 0.0f, 100.0f, ekg::dock::top | ekg::dock::left);
    this->p_slider_noise_control_offset->set_precision(2);

    ekg::label("Fog Dist:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_fog_dist = ekg::slider("FogDist", 512.0f, 0.0f, 1024.0f, ekg::dock::top | ekg::dock::left);
    this->p_slider_fog_dist->set_precision(2);

    this->p_checkbox_post_processing = ekg::checkbox("Post processing effects", ekg::dock::top | ekg::dock::left | ekg::dock::next);

    ekg::button("Reset light-spot", ekg::dock::top | ekg::dock::left | ekg::dock::next)->set_callback(new ekg::cpu::event {"callback", this, [](void *p_data) {
        auto starter {static_cast<client::scenes::starter*>(p_data)};
        starter->p_light_spot->position = api::world::current_player()->position;
        starter->p_light_spot->update();
    }});

    ekg::button("Reset dinossaurinho", ekg::dock::top | ekg::dock::left | ekg::dock::next)->set_callback(new ekg::cpu::event {"callback22", this, [](void *p_data) {
        auto starter {static_cast<client::scenes::starter*>(p_data)};
        auto &p {api::world::current_player()};

        starter->p_object_dino->position = p->position;
        starter->p_object_dino->rotation = p->rotation;
    }});
}

void client::scenes::starter::on_destroy() {

}

void client::scenes::starter::on_event(SDL_Event &sdl_event) {
    auto camera {api::world::current_camera()};
    ekg::event(sdl_event);

    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {

            break;
        }

        case SDL_MOUSEBUTTONUP: {

            break;
        }

        case SDL_KEYDOWN: {

            break;
        }

        case SDL_MOUSEMOTION: {
            if (api::input::pressed("mouse-right")) {

            }
            break;
        }
    }
}

void client::scenes::starter::on_update() {
    glm::vec3 intensity {glm::vec3(this->p_slider_light_intensity->get_value())};

    if (this->p_light_spot->intensity != intensity) {
        this->p_light_spot->intensity = intensity;
        this->p_light_spot->update();
    }

    auto &w {api::world::get()};
    w->config_chunk_gen_dist.set_value((int32_t) this->p_slider_range->get_value());
    w->config_chunk_noise.set_value({this->p_slider_noise_control_x->get_value(), this->p_slider_noise_control_y->get_value()});
    w->config_chunk_noise_offset.set_value(this->p_slider_noise_control_offset->get_value());

    auto &r {api::world::renderer()};
    r->config_fog_distance.set_value({0, this->p_slider_fog_dist->get_value()});
    r->config_post_processing.set_value(this->p_checkbox_post_processing->get_value());

    ekg::display::dt = api::dt;
    ekg::update();
}

void client::scenes::starter::on_render() {
    std::string chunk_info_text {"chunks count drawing "};
    chunk_info_text += std::to_string(api::world::renderer()->wf_chunk_draw_list.size());

    std::string info_hud[] {
            chunk_info_text,
            "Press WASD to move camera!",
            "Press H to change mascot position/rotation!",
            "Press mouse-2 (middle click) to change sun light position!",
            "Press T to enable post processing effect!"
    };

    ekg::render();
}