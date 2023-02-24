#include "sky.hpp"
#include "util/math.hpp"
#include "agk.hpp"

void sky::on_create() {
    /* I made some tests with colors and these colors seems cool to simulate sky world time. */
    this->color_day = {0.30196078431372547f, 0.6196078431372549f, 0.9803921568627451f};
    this->color_night = {0.0f, 0.0f, 0.08117647058827604f};

    // The start hour is 6am.
    this->day_ambient_light = 0.4f;
    this->night_ambient_light = 0.01f;
    this->set_time(6, 0);
}

void sky::on_destroy() {
}

void sky::on_event(SDL_Event &sdl_event) {
}

void sky::on_update() {
    /* Add + 1 (true), else false add 0. */
    this->delta_si_real_elapsed += static_cast<uint64_t>(util::reset_when(this->delta_ms_real, 1000));
    this->delta_hour_virtual = this->delta_si_real_elapsed / 60;
    this->delta_min_virtual = this->delta_si_real_elapsed;
    this->ambient_luminance = this->day_ambient_light;

    if (this->delta_hour_virtual > 24) {
        this->delta_hour_virtual = 0;
        this->delta_min_virtual = 0;
        this->delta_si_real_elapsed = 0;
        this->delta_day_virtual++;

        std::string msg {std::to_string(this->delta_day_virtual)}; msg += " day(s) passed.";
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

            /* The initial difference from the current color position between the k is needed. */
            this->color_from_sky = (this->color_night) + (k * this->color_day);
            this->color_from_sky = glm::clamp(this->color_from_sky, this->color_night, this->color_day);
            this->ambient_next_light = glm::clamp(k * this->day_ambient_light, this->night_ambient_light, this->day_ambient_light);
            break;
        }

        case 18: {
            this->is_night = true;

            /* Here is the same code except the part to set the new sky color. */
            uint64_t delta_si_h = {this->delta_hour_virtual * 60};
            uint64_t delta_i {this->delta_min_virtual - delta_si_h};
            float k {static_cast<float>(delta_i) / 60};

            /* As you can see we need to subtract the "initial" night color with the current degree of decrease day color. */
            this->color_from_sky = glm::clamp((1.0f - k) * this->color_day, this->color_night, this->color_day);
            this->ambient_next_light = glm::clamp((1.0f - k) * this->day_ambient_light, this->night_ambient_light, this->day_ambient_light);
            break;
        }

        case 19: {
            this->color_from_sky = this->color_night;
            break;
        }

        case 7: {
            this->color_from_sky = this->color_day;
            break;
        }
    }

    /* Set the sky and ambient light using interpolation to degree smooth.  */
    agk::app.background[0] = agk::app.background[0] + (this->color_from_sky.x - agk::app.background[0]) * agk::dt;
    agk::app.background[1] = agk::app.background[1] + (this->color_from_sky.y - agk::app.background[1]) * agk::dt;
    agk::app.background[2] = agk::app.background[2] + (this->color_from_sky.z - agk::app.background[2]) * agk::dt;

    /* The ambient light is a high-precision float, it is cool to have a smooth degree. */
    this->ambient_light = this->ambient_light + (this->ambient_next_light - this->ambient_light) * agk::dt;
    agk::app.setting.fog_color.set_value(this->color_from_sky);
}

void sky::on_render() {
    // @todo Add skybox, dynamic clouds, parallax sun and parallax moon.
}

void sky::set_time(int32_t hour, int32_t minutes) {
    if (hour > 23) {
        hour = 0;
    }

    util::reset(this->delta_ms_real);

    if (hour > 6 && hour < 19) {
        this->is_night = false;
        this->color_from_sky = this->color_day;
        this->ambient_light = this->day_ambient_light;
    } else {
        this->is_night = true;
        this->color_from_sky = this->color_night;
        this->ambient_light = this->night_ambient_light;
    }

    this->delta_si_real_elapsed = (60 * hour) + minutes;
    this->ambient_next_light = this->ambient_light;
}
