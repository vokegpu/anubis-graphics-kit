#include "starter.hpp"
#include "api/gpu/tools.hpp"
#include "api/util/env.hpp"
#include "api/api.hpp"
#include "api/world/environment/env_object.hpp"
#include "api/world/environment/env_light.hpp"

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

    ekg::gl_version = "#version 450 core";
    ekg::init(api::app.p_sdl_window, "./data/fonts/JetBrainsMono-Bold.ttf");

    auto frame = ekg::frame("Hello", {20, 20}, {330, 400});
    frame->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::right);
    frame->set_drag(ekg::dock::top);

    ekg::label("Welcome to AGK!", ekg::dock::top | ekg::dock::left);
    ekg::label("Light Intensity:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_light_intensity = ekg::slider("LightIntensity", 50.0f, 0.0f, 4024.0f, ekg::dock::top | ekg::dock::left);
    this->p_slider_light_intensity->set_precision(2);
    this->p_slider_light_intensity->set_value(0.8f);

    ekg::label("Chunk Range:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_range = ekg::slider("ChunkRange", 3, 1, 16, ekg::dock::top | ekg::dock::left);
    this->p_slider_range->set_precision(1);
    this->p_slider_range->set_value(3);

    ekg::label("Fog Dist:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_fog_dist = ekg::slider("FogDist", 1024.0f, 0.0f, 1024.0f * 4, ekg::dock::top | ekg::dock::left);
    this->p_slider_fog_dist->set_precision(2);
    this->p_slider_fog_dist->set_value(2048.0f);

    ekg::label("Chunking Frequency:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_frequency = ekg::slider("Frequency", 0.15f, 0.0f, 1.0f, ekg::dock::top | ekg::dock::left);
    this->p_frequency->set_value(0.66260f);
    this->p_frequency->set_precision(3);

    ekg::label("Chunking Amplitude:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_amplitude = ekg::slider("Amplitude", 0.15f, 0.0f, 1.0f, ekg::dock::top | ekg::dock::left);
    this->p_amplitude->set_value(0.35206f);
    this->p_amplitude->set_precision(3);

    ekg::label("Chunking Persistence:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_persistence = ekg::slider("Persistence", 0.15f, 0.0f, 1.0f, ekg::dock::top | ekg::dock::left);
    this->p_persistence->set_value(0.51004f);
    this->p_persistence->set_precision(3);
    
    ekg::label("Chunking Lacunarity:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_lacunarity = ekg::slider("Lacunarity", 0.15f, 0.0f, 1.0f, ekg::dock::top | ekg::dock::left);
    this->p_lacunarity->set_value(0.15f);
    this->p_lacunarity->set_precision(3);

    this->p_octaves = ekg::slider("Octaves", 2, 0, 30, ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_octaves->set_value(6);
    this->p_octaves->set_precision(1);

    this->p_checkbox_post_processing = ekg::checkbox("Post processing effects", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_check_box_hdr = ekg::checkbox("Enable HDR", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_slider_hdr_exposure = ekg::slider("Exposure HDR", 0.43f, 0.0f, 3.0f, ekg::dock::top | ekg::dock::left);
    this->p_slider_hdr_exposure->set_value(0.43f);
    this->p_slider_hdr_exposure->set_precision(2);

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

    SDL_SetWindowSize(api::app.p_sdl_window, 1920, 1080);
    SDL_SetWindowFullscreen(api::app.p_sdl_window, true);
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
    if (api::input::pressed("x")) {
        api::world::current_player()->speed_base += 0.5f;
    }

    if (api::input::pressed("z")) {
        api::world::current_player()->speed_base -= 0.5f;
    }

    if (this->last_display_fps != api::app.display_fps) {
        std::string title {"Anubis Graphics Kit "};
        title += std::to_string(api::app.display_fps);
        title += "fps";
        SDL_SetWindowTitle(api::app.p_sdl_window, title.c_str());
        this->last_display_fps = api::app.display_fps;
    }

    glm::vec3 intensity {glm::vec3(this->p_slider_light_intensity->get_value())};

    if (this->p_light_spot->intensity != intensity) {
        this->p_light_spot->intensity = intensity;
        this->p_light_spot->update();
    }

    auto &w {api::world::get()};
    w->config_chunk_gen_dist.set_value((int32_t) this->p_slider_range->get_value());
    w->config_chunk_frequency.set_value(this->p_frequency->get_value());
    w->config_chunk_amplitude.set_value(this->p_amplitude->get_value());
    w->config_chunk_persistence.set_value(this->p_persistence->get_value());
    w->config_chunk_lacunarity.set_value(this->p_lacunarity->get_value());
    w->config_chunk_octaves.set_value((int32_t) this->p_octaves->get_value());

    auto &r {api::world::renderer()};
    r->config_fog_distance.set_value({0, this->p_slider_fog_dist->get_value()});
    r->config_post_processing.set_value(this->p_checkbox_post_processing->get_value());
    r->config_hdr.set_value(this->p_check_box_hdr->get_value());
    r->config_hdr_exposure.set_value(this->p_slider_hdr_exposure->get_value());

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