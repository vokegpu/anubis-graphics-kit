#include "starter.hpp"
#include "gpu/tools.hpp"
#include "util/env.hpp"
#include "agk.hpp"
#include "world/environment/object.hpp"
#include "world/environment/light.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

void client::scenes::starter::on_create() {
    agk::pbr::loadmodel("scene", "./data/models/scene.gltf");
    agk::pbr::loadmaterial("scene", "./data/models/scene.gltf");

    agk::pbr::loadmodel("dinossaur", "./data/models/Dinossaur.stl");
    agk::pbr::loadmaterial("dinossaur", new material {{
        {"color", stream::vec(0.942f, 0.23434f, 0.71334f)},
        {"metal", stream::i(false)},
        {"rough", stream::f(0.80)},
        {"doubleSided", stream::i(true)}
    }});

    agk::pbr::loadmodel("monster", "./data/models/Alien Animal.obj");
    agk::pbr::loadmaterial("monster", new material {{
        {"color", stream::vec(0.243534f, 0.43545f, 0.03f)},
        {"metal", stream::i(true)},
        {"rough", stream::f(0.20)},
        {"doubleSided", stream::i(true)}
    }});

    agk::pbr::loadmodel("coconuttree", "./data/models/Coconut Tree.obj");
    agk::pbr::loadmodel("monster", "./data/models/Alien Animal.obj");
    agk::pbr::loadmaterial("snowball", "./data/models/snow_03_4k.gltf");

    auto *p_dinossaur {new object {}};
    p_dinossaur->assign("model.scene", "material.scene");
    p_dinossaur->transform.position.y = 270.0f;
    p_dinossaur->transform.scale = glm::vec3 {2.0f};
    p_dinossaur->transform.rotation.x = -glm::radians(90.0f);
    agk::world::create(p_dinossaur);

    auto *p_gltf_obj1 {new object {}};
    p_gltf_obj1->transform.position = {0, 300, 0};
    p_gltf_obj1->transform.scale = glm::vec3 {30.1343400f};
    p_gltf_obj1->assign("model.snowball", "material.snowball");
    agk::world::create(p_gltf_obj1);

    auto *p_monster {new object {}};
    p_monster->transform.position.y = 720.0f + 500.0f;
    p_monster->transform.scale = glm::vec3 {720.0f};
    agk::world::create(p_monster);

    this->p_light_spot = new light {};
    this->p_light_spot->intensity = {300, 300, 300};
    this->p_light_spot->transform.position = {0, 300, 0};
    this->p_light_spot->update();

    auto *p_light_spot2 = new light {};
    p_light_spot2->intensity = {50, 50, 50};
    p_light_spot2->transform.position = {0, 15, 0};
    p_light_spot2->update();

    agk::world::create(this->p_light_spot);
    agk::world::create(p_light_spot2);
    agk::world::currentplayer()->speed_base = 0.9540f;

    auto &p_user_camera {agk::ui::getusercamera()};
    p_user_camera->set_editor_enabled(true);
    p_user_camera->set_movement_enabled(true);
    p_user_camera->set_rotation_enabled(true);
    p_user_camera->bind_editor_rotate.set_value("mouse-2");

    ekg::gl_version = "#version 450";
    ekg::init(agk::app.p_sdl_window, "./data/fonts/JetBrainsMono-Bold.ttf");

    auto frame = ekg::frame("Hello", {20, 600}, {620, 330});
    frame->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::right);
    frame->set_drag(ekg::dock::top);

    ekg::button("Reset light-spot", ekg::dock::fill)->set_callback(new ekg::cpu::event {"callback", this, [](void *p_data) {
        auto starter {static_cast<client::scenes::starter*>(p_data)};
        starter->p_light_spot->transform.position = agk::world::currentplayer()->transform.position;
        starter->p_light_spot->update();
    }});

    agk::setfps(60, true);
    ekg::button("Vsync", ekg::dock::fill)->set_callback(new ekg::cpu::event {"callback23", nullptr, [](void *p_data) {
        agk::setfps(60, !agk::app.vsync);
    }});

    ekg::label("Time Now:", ekg::dock::fill | ekg::dock::next);
    this->p_time_now = ekg::slider("Time", 17, 0, 23, ekg::dock::fill);
    this->p_time_now->set_precision(0);

    ekg::label("Light Intensity:", ekg::dock::fill | ekg::dock::next)->set_font_size(ekg::font::small);
    this->p_light_intensity = ekg::slider("LightIntensity", 0.233f, 0.0f, 4024.0f, ekg::dock::fill);
    this->p_light_intensity->set_precision(2);

    ekg::label("Chunk Range:", ekg::dock::fill | ekg::dock::next);
    this->p_chunk_range = ekg::slider("ChunkRange", 6, 1, 16, ekg::dock::fill);

    ekg::label("Fog Dist:", ekg::dock::fill | ekg::dock::next);
    this->p_fog_distance = ekg::slider("FogDist", 1024.0f * 4, 0.0f, 1024.0f * 16, ekg::dock::fill);
    this->p_fog_distance->set_precision(2);

    ekg::label("FBM Frequency:", ekg::dock::fill | ekg::dock::next);
    this->p_frequency = ekg::slider("Frequency", 0.34105f, 0.0f, 1.0f, ekg::dock::fill);
    this->p_frequency->set_precision(3);

    ekg::label("FBM Amplitude:", ekg::dock::fill | ekg::dock::next);
    this->p_amplitude = ekg::slider("Amplitude", 0.18874f, 0.0f, 1.0f, ekg::dock::fill);
    this->p_amplitude->set_precision(3);

    ekg::label("FBM Persistence:", ekg::dock::fill | ekg::dock::next);
    this->p_persistence = ekg::slider("Persistence", 0.28834f, 0.0f, 1.0f, ekg::dock::fill);
    this->p_persistence->set_precision(3);
    
    ekg::label("FBM Lacunarity:", ekg::dock::fill | ekg::dock::next);
    this->p_lacunarity = ekg::slider("Lacunarity", 0.29888f, 0.0f, 1.0f, ekg::dock::fill);
    this->p_lacunarity->set_precision(3);

    ekg::label("FBM Octaves:", ekg::dock::fill | ekg::dock::next);
    this->p_octaves = ekg::slider("Octaves", 5, 0, 30, ekg::dock::fill);

    auto p_frame {ekg::frame("hello vc é fofo", {1200, 600}, {300, 200})};
    p_frame->set_resize(ekg::dock::left | ekg::dock::bottom | ekg::dock::right);
    p_frame->set_drag(ekg::dock::top);

    this->p_enable_post_processing = ekg::checkbox("Post processing effects", ekg::dock::fill);
    this->p_enable_post_processing->set_value(true);

    ekg::label("HDR:", ekg::dock::fill | ekg::dock::next);
    this->p_enable_hdr = ekg::checkbox("Enable", ekg::dock::fill);
    ekg::label("Exposure:", ekg::dock::fill | ekg::dock::next);
    this->p_hdr_exposure = ekg::slider("Exposure", 0.43f, 0.0f, 3.0f, ekg::dock::fill);
    this->p_hdr_exposure->set_precision(2);

    ekg::label("Motion Blur:", ekg::dock::fill | ekg::dock::next);
    this->p_enable_motion_blur = ekg::checkbox("Enable", ekg::dock::fill);
    this->p_enable_motion_blur->set_value(true);
    ekg::label("Intensity:", ekg::dock::fill | ekg::dock::next);
    this->p_motion_blur_intensity = ekg::slider("Intensity", 0.151f, 0.0f, 1.0f, ekg::dock::fill);
    this->p_motion_blur_intensity->set_precision(2);

    ekg::debug = false;

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

    agk::world::sky()->set_time(18, 0);
    agk::world::currentplayer()->transform.position.y += 90;
    this->do_json_test();
}

void client::scenes::starter::do_json_test() {
    std::ifstream ifs {"./data/dev/cat.json"};
    nlohmann::json json = nlohmann::json::parse(ifs);
    ifs.close();
}

void client::scenes::starter::on_destroy() {

}

void client::scenes::starter::on_event(SDL_Event &sdl_event) {
    ekg::event(sdl_event);
}

void client::scenes::starter::on_update() {
    if (agk::ui::input("x")) {
        agk::world::currentplayer()->speed_base += 0.5f;
    }

    if (agk::ui::input("z")) {
        agk::world::currentplayer()->speed_base -= 0.5f;
    }

    if (this->p_time_now->is_dragging()) {
        agk::world::sky()->set_time(static_cast<int32_t>(this->p_time_now->get_value()), 0);
    } else if (static_cast<uint64_t>(this->p_time_now->get_value()) != sky::hour) {
        this->p_time_now->set_value(static_cast<float>(sky::hour));
    }

    if (this->last_display_fps != agk::app.display_fps) {
        std::string title {"Anubis Graphics Kit "};
        title += std::to_string(agk::app.display_fps);
        title += "fps";
        title += agk::app.vsync ? "+vsync" : "";
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

    glm::vec2 fog_bounding {this->p_fog_distance->get_value() * 0.5f, this->p_fog_distance->get_value()};
    agk::app.setting.fog_bounding.set_value(fog_bounding);
    if (this->p_fog_distance->is_dragging()) {
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
