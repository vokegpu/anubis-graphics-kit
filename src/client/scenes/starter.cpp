#include "starter.hpp"
#include "gpu/tools.hpp"
#include "util/env.hpp"
#include "agk.hpp"
#include "world/environment/object.hpp"
#include "world/environment/light.hpp"

void client::scenes::starter::on_create() {
    asset::model *p_model_dino {new asset::model {"dinossaur", "./data/models/dinossaur.stl", glm::ivec4(GL_STATIC_DRAW)}};
    agk::asset::load(p_model_dino);

    material *p_material {new material(enums::material::dialetric)};
    p_material->append_mtl((::asset::model*) agk::asset::find("models/vegetation.coconut"));

    for (int i {}; i < 1; i++) {
        this->p_object_dino = new object(p_model_dino);
        p_material->set_color({1.0f, 215.0f / 255.0f, 0.0f});
        this->p_object_dino->p_material = p_material;
        this->p_object_dino->transform.scale = {1.0f, 1.0f, 1.0f};
        agk::world::create(this->p_object_dino);
    }

    this->p_light_spot = new light(nullptr);
    this->p_light_spot->intensity = {300, 300, 300};
    this->p_light_spot->transform.position = {0, 300, 0};
    this->p_light_spot->update();

    auto p_light_spot2 = new light(nullptr);
    p_light_spot2->intensity = {50, 50, 50};
    p_light_spot2->transform.position = {0, 15, 0};
    p_light_spot2->update();

    agk::world::create(this->p_light_spot);
    agk::world::create(p_light_spot2);
    agk::world::current_player()->speed_base = 0.9540f;

    this->p_camera_manager->set_editor_enabled(true);
    this->p_camera_manager->set_movement_enabled(true);
    this->p_camera_manager->set_rotation_enabled(true);
    this->p_camera_manager->bind_editor_rotate.set_value("mouse-2");

    ekg::gl_version = "#version 450";
    ekg::init(agk::app.p_sdl_window, "./data/fonts/JetBrainsMono-Bold.ttf");

    auto frame = ekg::frame("Hello", {20, 20}, {330, 400});
    frame->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::right);
    frame->set_drag(ekg::dock::top);

    ekg::button("Reset light-spot", ekg::dock::top | ekg::dock::left | ekg::dock::next)->set_callback(new ekg::cpu::event {"callback", this, [](void *p_data) {
        auto starter {static_cast<client::scenes::starter*>(p_data)};
        starter->p_light_spot->transform.position = agk::world::current_player()->transform.position;
        starter->p_light_spot->update();
    }});

    ekg::button("Reset Mascote", ekg::dock::top | ekg::dock::left | ekg::dock::next)->set_callback(new ekg::cpu::event {"callback22", this, [](void *p_data) {
        auto starter {static_cast<client::scenes::starter*>(p_data)};
        auto &p {agk::world::current_player()};

        starter->p_object_dino->transform.position = p->transform.position;
        starter->p_object_dino->transform.rotation = p->transform.rotation;
    }});

    ekg::label("Light Intensity:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_light_intensity = ekg::slider("LightIntensity", 0.233f, 0.0f, 4024.0f, ekg::dock::top | ekg::dock::left);
    this->p_light_intensity->set_precision(2);

    ekg::label("Chunk Range:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_chunk_range = ekg::slider("ChunkRange", 3, 1, 16, ekg::dock::top | ekg::dock::left);

    ekg::label("Fog Dist:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_fog_distance = ekg::slider("FogDist", 1024.0f * 4, 0.0f, 1024.0f * 16, ekg::dock::top | ekg::dock::left);
    this->p_fog_distance->set_precision(2);

    ekg::label("FBM Frequency:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_frequency = ekg::slider("Frequency", 0.15f, 0.0f, 1.0f, ekg::dock::top | ekg::dock::left);
    this->p_frequency->set_precision(3);

    ekg::label("FBM Amplitude:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_amplitude = ekg::slider("Amplitude", 0.15f, 0.0f, 1.0f, ekg::dock::top | ekg::dock::left);
    this->p_amplitude->set_precision(3);

    ekg::label("FBM Persistence:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_persistence = ekg::slider("Persistence", 0.15f, 0.0f, 1.0f, ekg::dock::top | ekg::dock::left);
    this->p_persistence->set_precision(3);
    
    ekg::label("FBM Lacunarity:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_lacunarity = ekg::slider("Lacunarity", 0.15f, 0.0f, 1.0f, ekg::dock::top | ekg::dock::left);
    this->p_lacunarity->set_precision(3);

    ekg::label("FBM Octaves:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_octaves = ekg::slider("Octaves", 5, 0, 30, ekg::dock::top | ekg::dock::left);

    auto p_frame {ekg::frame("hello vc é fofo", {500, 20}, {300, 200})};
    p_frame->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);
    p_frame->set_drag(ekg::dock::top);

    this->p_enable_post_processing = ekg::checkbox("Post processing effects", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_enable_post_processing->set_value(true);

    ekg::label("HDR:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_enable_hdr = ekg::checkbox("Enable", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    ekg::label("Exposure:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_hdr_exposure = ekg::slider("Exposure", 0.43f, 0.0f, 3.0f, ekg::dock::top | ekg::dock::left);
    this->p_hdr_exposure->set_precision(2);

    ekg::label("Motion Blur:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_enable_motion_blur = ekg::checkbox("Enable", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_enable_motion_blur->set_value(true);
    ekg::label("Intensity:", ekg::dock::top | ekg::dock::left | ekg::dock::next);
    this->p_motion_blur_intensity = ekg::slider("Intensity", 0.351f, 0.0f, 1.0f, ekg::dock::top | ekg::dock::left);
    this->p_motion_blur_intensity->set_precision(2);

    float mesh[] {
        -0.2f, -0.2f,
        -0.4f, -0.2f,
        -0.2f, -0.4f,
        -0.4f, -0.4f,
        0.2f, 0.2f,
        0.4f, 0.2f,
        0.2f, 0.4f,
        0.4f, 0.4f,
    };

    uint8_t indices[] {
        0, 1, 3,
        3, 2, 0,
        4 + 0, 4 + 1, 4 + 3,
        4 + 3, 4 + 2, 4 + 0
    };

    this->buffer_test.invoke();
    this->buffer_test.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_test.send<float>(sizeof(mesh), mesh, GL_STATIC_DRAW);
    this->buffer_test.attach(0, 2);

    this->buffer_test.bind(1, {GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_BYTE});
    this->buffer_test.send<uint8_t>(sizeof(indices), indices, GL_STATIC_DRAW);
    this->buffer_test.revoke();
    this->buffer_test.primitive[0] = GL_TRIANGLES;

    agk::asset::load(new ::asset::shader {"effects.overlay.debug", {
            {"./data/effects/overlay.debug.vert", GL_VERTEX_SHADER},
            {"./data/effects/overlay.debug.frag", GL_FRAGMENT_SHADER}
    }});

    agk::world::sky()->set_time(8, 0);
    agk::world::current_player()->transform.position.y += 90;
}

void client::scenes::starter::on_destroy() {

}

void client::scenes::starter::on_event(SDL_Event &sdl_event) {
    auto camera {agk::world::current_camera()};
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
            break;
        }
    }
}

void client::scenes::starter::on_update() {
    if (agk::input::pressed("x")) {
        agk::world::current_player()->speed_base += 0.5f;
    }

    if (agk::input::pressed("z")) {
        agk::world::current_player()->speed_base -= 0.5f;
    }

    if (this->last_display_fps != agk::app.display_fps) {
        std::string title {"Anubis Graphics Kit "};
        title += std::to_string(agk::app.display_fps);
        title += "fps";
        SDL_SetWindowTitle(agk::app.p_sdl_window, title.c_str());
        this->last_display_fps = agk::app.display_fps;
    }

    glm::vec3 intensity {glm::vec3(this->p_light_intensity->get_value())};

    if (this->p_light_spot->intensity != intensity) {
        this->p_light_spot->intensity = intensity;
        this->p_light_spot->update();
    }

    agk::app.setting.chunk_generation_distance.set_value((int32_t) this->p_chunk_range->get_value());
    agk::app.setting.chunk_fbm_frequency.set_value(this->p_frequency->get_value());
    agk::app.setting.chunk_fbm_amplitude.set_value(this->p_amplitude->get_value());
    agk::app.setting.chunk_fbm_persistence.set_value(this->p_persistence->get_value());
    agk::app.setting.chunk_fbm_lacunarity.set_value(this->p_lacunarity->get_value());
    agk::app.setting.chunk_fbm_octaves.set_value((int32_t) this->p_octaves->get_value());

    glm::vec2 fog_bounding {0, this->p_fog_distance->get_value()};
    agk::app.setting.fog_bounding.set_value(fog_bounding);
    if (agk_perspective_clip_distance != fog_bounding.y) {
        agk_perspective_clip_distance = fog_bounding.y;
        agk::app.p_curr_camera->process_perspective(agk::app.screen_width, agk::app.screen_height);
    }

    agk::app.setting.enable_post_processing.set_value(this->p_enable_post_processing->get_value());
    agk::app.setting.enable_hdr.set_value(this->p_enable_hdr->get_value());
    agk::app.setting.hdr_exposure.set_value(this->p_hdr_exposure->get_value());
    agk::app.setting.enable_motion_blur.set_value(this->p_enable_motion_blur->get_value());
    agk::app.setting.motion_blur_intensity.set_value(this->p_motion_blur_intensity->get_value());

    ekg::display::dt = agk::dt;
    ekg::update();
}

void client::scenes::starter::on_render() {
    ekg::render();
    // this->do_test_overlay();
}

void client::scenes::starter::do_test_overlay() {
    auto *p_program {(::asset::shader*) agk::asset::find("gpu/effects.overlay.debug")};
    p_program->invoke();
    this->buffer_test.invoke();

    this->buffer_test.stride[0] = 0;
    this->buffer_test.stride[1] = 12;
    this->buffer_test.draw();

    this->buffer_test.revoke();
    p_program->revoke();
}
