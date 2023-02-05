#include "env_world_time_manager.hpp"
#include "api/util/math.hpp"
#include "api/api.hpp"

void world_time_manager::on_create() {
    feature::on_create();

    /* I made some tests with colors and these colors seems cool to simulate sky world time. */
    this->color_day = {0.30196078431372547f, 0.6196078431372549f, 0.9803921568627451f};
    this->color_nightmare = {0.0f, 0.0f, 0.08117647058827604f};

    // The start hour is 6am.
    this->delta_si_real_elapsed = 60 * 7;
    this->color_from_sky = this->color_day;
    this->day_ambient_light = 0.4f;
    this->nightmare_ambient_light = 0.0f;
    this->ambient_light = this->day_ambient_light;
    this->ambient_next_light = this->ambient_light;
}

void world_time_manager::on_destroy() {
    feature::on_destroy();
}

void world_time_manager::on_event(SDL_Event &sdl_event) {
    feature::on_event(sdl_event);
}

void world_time_manager::on_update() {
    feature::on_update();

    /* Add + 1 (true), else false add 0. */
    this->delta_si_real_elapsed += static_cast<uint64_t>(util::reset_when(this->delta_ms_real, 1000));
    this->delta_hour_virtual = this->delta_si_real_elapsed / 60;
    this->delta_min_virtual = this->delta_si_real_elapsed;

    if (this->delta_hour_virtual > 24) {
        this->delta_hour_virtual = 0;
        this->delta_min_virtual = 0;
        this->delta_si_real_elapsed = 0;
        this->delta_day_virtual++;

        std::string msg {std::to_string(this->delta_day_virtual)}; msg += " day(s) have been passed.";
        util::log(msg);
    }

    /* Processing background based in middle temporal delta. */
    /* Perhaps I am going to write a research article explaining Rina's equation used here. */
    switch (this->delta_hour_virtual) {
        case 6: {
            this->is_night = false;

            uint64_t delta_si_h {this->delta_hour_virtual * 60};
            uint64_t delta_i {this->delta_min_virtual - delta_si_h};
            float k {static_cast<float>(delta_i) / 60};

            /* The initial difference from the current color position between the 0.0f is needed. */
            this->color_from_sky = (this->color_nightmare) + (k * this->color_day);
            this->color_from_sky = glm::clamp(this->color_from_sky, this->color_nightmare, this->color_day);
            this->ambient_next_light = glm::clamp(k * this->day_ambient_light, this->nightmare_ambient_light, this->day_ambient_light);
            break;
        }

        case 18: {
            this->is_night = true;

            /* Here is the same code except the part to set the new sky color. */
            uint64_t delta_si_h = {this->delta_hour_virtual * 60};
            uint64_t delta_i {this->delta_min_virtual - delta_si_h};
            float k {static_cast<float>(delta_i) / 60};

            /* As you can see we need to subtract the "initial" nightmare color with the current degree of decrease day color. */
            this->color_from_sky = glm::clamp((1.0f - k) * this->color_day, this->color_nightmare, this->color_day);
            this->ambient_next_light = glm::clamp((1.0f - k) * this->day_ambient_light, this->nightmare_ambient_light, this->day_ambient_light);
            break;
        }

        case 19: {
            this->color_from_sky = this->color_nightmare;
            break;
        }

        case 7: {
            this->color_from_sky = this->color_day;
            break;
        }
    }

    /* Set the sky and ambient light using interpolation to degree smooth.  */
    api::app.background[0] = api::app.background[0] + (this->color_from_sky.x - api::app.background[0]) * api::dt;
    api::app.background[1] = api::app.background[1] + (this->color_from_sky.y - api::app.background[1]) * api::dt;
    api::app.background[2] = api::app.background[2] + (this->color_from_sky.z - api::app.background[2]) * api::dt;

    /* The ambient light is a high-precision float, it is cool to have a smooth degree. */
    this->ambient_light = this->ambient_light + (this->ambient_next_light - this->ambient_light) * api::dt;

    auto &p_world_renderer {api::world::renderer()};
    p_world_renderer->config_fog_color.set_value(this->color_from_sky);
}

void world_time_manager::on_render() {
    feature::on_render();

    // @todo Add skybox, dynamic clouds, parallax sun and parallax moon.
}