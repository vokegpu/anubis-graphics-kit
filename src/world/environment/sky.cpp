#include "sky.hpp"
#include "util/math.hpp"
#include "agk.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>

bool sky::isnight {};
uint64_t sky::hour {};

void sky::on_create() {
    /* I made some tests with colors and these colors seems cool to simulate sky world time. */
    this->color_day = {0.30196078431372547f, 0.6196078431372549f, 0.9803921568627451f};
    this->color_night = {0.0f, 0.0f, 0.08117647058827604f};

    // The start hour is 6am.
    this->day_ambient_light = 0.4f;
    this->night_ambient_light = 0.01f;
    this->set_time(6, 0);

    this->p_astroo_light = new light {};
    this->p_astroo_light->directional = true;
    this->p_astroo_light->intensity = {10.08117647058827604f, 0.0f, 0.0f};
    this->p_astroo_light->update();
    agk::world::create(this->p_astroo_light);

    stream::mesh tesseract_mesh {};
    glm::vec3 tesseract_cubic_massive {32, 64, 16};
    glm::vec3 p {};

    glm::vec4 p4 {};
    glm::vec3 r {};

    float noised {};
    std::srand(std::time(nullptr));

    for (int32_t x {}; x < tesseract_cubic_massive.x; x++) {
        for (int32_t y {}; y < tesseract_cubic_massive.y; y++) {
            for (int32_t z {}; z < tesseract_cubic_massive.z; z++) {
                p.x = static_cast<float>(x) / static_cast<float>(tesseract_cubic_massive.x);
                p.y = static_cast<float>(y) / static_cast<float>(tesseract_cubic_massive.y);
                p.z = static_cast<float>(z) / static_cast<float>(tesseract_cubic_massive.z);

                p.x -= ((std::rand() % static_cast<int32_t>(tesseract_cubic_massive.z)) / static_cast<float>(tesseract_cubic_massive.z * 2.0f));
                p.y -= ((std::rand() % static_cast<int32_t>(tesseract_cubic_massive.z)) / static_cast<float>(tesseract_cubic_massive.z * 2.0f));
                p.z -= ((std::rand() % static_cast<int32_t>(tesseract_cubic_massive.z)) / static_cast<float>(tesseract_cubic_massive.z * 2.0f));

                noised = glm::simplex(p);
                p.x += noised;

                noised = glm::simplex(p);
                p.y += noised;

                noised = glm::simplex(p);
                p.z += noised;

                p4.x = p.y;
                p4.y = p.z;
                p4.z = p.x;
                p4.w = ((std::rand() % static_cast<int32_t>(tesseract_cubic_massive.z)) / static_cast<float>(tesseract_cubic_massive.z * 2.0f));

                noised = glm::simplex(-p4);
                p += noised;

                //r.x = ((std::rand() % static_cast<int32_t>(tesseract_cubic_massive.x)) / static_cast<float>(tesseract_cubic_massive.x * 8.0f));
                //r.y = ((std::rand() % static_cast<int32_t>(tesseract_cubic_massive.y)) / static_cast<float>(tesseract_cubic_massive.y * 8.0f));
                //r.z = ((std::rand() % static_cast<int32_t>(tesseract_cubic_massive.z)) / static_cast<float>(tesseract_cubic_massive.z * 8.0f));

                //r = (r / 2.0f) - r;
                //p += r;

                tesseract_mesh.append(p, stream::type::vertex);
                tesseract_mesh.faces++;
            }
        }
    }

    auto &v_list {tesseract_mesh.get_float_list(stream::type::vertex)};

    this->buffer_tesseract.primitive[0] = GL_POINTS;
    this->buffer_tesseract.stride[0] = 0;
    this->buffer_tesseract.stride[1] = v_list.size();
    this->buffer_tesseract.invoke();
    this->buffer_tesseract.bind(0, {GL_ARRAY_BUFFER, GL_FLOAT});
    this->buffer_tesseract.send<float>(sizeof(float)*v_list.size(), v_list.data(), GL_STATIC_DRAW);
    this->buffer_tesseract.attach(0, 3, {0, 0});
    this->buffer_tesseract.unbind();
    this->buffer_tesseract.revoke();

    agk::pbr::loadmodel("moonball", "./data/models/wood_table_001_4k.gltf");
    this->p_model_moon = (model*) agk::pbr::find("model.moonball.0");

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
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
    sky::isnight = this->is_night;
    sky::hour = this->delta_hour_virtual;
}

void sky::on_render() {
    if (this->is_night) {
        this->stars_luminance += 0.001f;
    } else {
        this->stars_luminance = glm::clamp(this->stars_luminance - 0.001f, 0.0f, 2.0f);
    }

    auto *&p_camera {agk::world::currentcamera()};
    auto *p_program {(::asset::shader*) agk::asset::find("gpu/effects.sky.pbr")};

    float clip_dist {256000.0f};
    float mid_clip_dist {clip_dist / 2.0f};

    glm::vec3 tesseract_pos {p_camera->transform.position};
    tesseract_pos.x -= mid_clip_dist;
    tesseract_pos.y = -1000.0f;
    tesseract_pos.z -= mid_clip_dist;

    glm::mat4 mat4x4rts {1.0f};
    glm::mat4 mat4x4projection {p_camera->get_mvp()};

    mat4x4rts = glm::translate(mat4x4rts, tesseract_pos);
    mat4x4rts = glm::scale(mat4x4rts, {clip_dist, clip_dist, clip_dist});
    mat4x4rts = mat4x4projection * mat4x4rts;

    p_program->invoke();
    p_program->set_uniform_mat4("uMVP", &mat4x4rts[0][0]);
    p_program->set_uniform_float("uStarsLuminance", this->stars_luminance);
    p_program->set_uniform_bool("uStarsRendering", true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    this->buffer_tesseract.invoke();
    this->buffer_tesseract.draw();
    this->buffer_tesseract.revoke();

    /*
     * z+ == west
     * z- == east
     * 
     * x+ == north
     * x- == south
     */
    glm::vec3 moon_color {0.6667f, 0.6667f, 0.6667f};
    glm::vec3 moon_pos {p_camera->transform.position};
    glm::vec3 moon_light_dir {0.90f, -0.50f, -0.30f};

    float factor {(static_cast<float>(this->delta_min_virtual)) / 1440};
    float dist {mid_clip_dist / 10.0f};

    if (factor < this->moon_pos_time.x) {
        this->moon_pos_time.z = -0.1f;
    } else {
        this->moon_pos_time.z = (this->moon_pos_time.z + ((factor * 360.0f) - this->moon_pos_time.z) * agk::dt);
    }

    this->moon_pos_time.x = factor;

    moon_pos.z -= dist;
    moon_pos.y += 9808.0f;
    dist /= 4;

    mat4x4rts = glm::mat4(1.0f);
    mat4x4rts = glm::rotate(mat4x4rts, glm::radians(this->moon_pos_time.z), {1.0f, 0.0f, 0.0f});
    
    this->p_astroo_light->transform.position = (glm::mat3(mat4x4rts) * moon_pos) - p_camera->transform.position;
    this->p_astroo_light->update();

    mat4x4rts = glm::translate(mat4x4rts, moon_pos);
    mat4x4rts = glm::scale(mat4x4rts, {dist, dist, dist});
    mat4x4rts = mat4x4projection * mat4x4rts;

    p_program->set_uniform_vec3("uColor", &moon_color[0]);
    p_program->set_uniform_mat4("uMVP", &mat4x4rts[0][0]);
    p_program->set_uniform_bool("uStarsRendering", false);
    p_program->set_uniform_vec3("uSkyColor", agk::app.background);
    p_program->set_uniform_vec3("uDirectionLightMoon", &moon_light_dir[0]);

    glDisable(GL_BLEND);

    this->p_model_moon->buffer.invoke();
    this->p_model_moon->buffer.draw();
    this->p_model_moon->buffer.revoke();

    p_program->revoke();
}

void sky::set_time(int32_t hours, int32_t minutes) {
    if (hours > 23) {
        hours = 0;
    }

    util::reset(this->delta_ms_real);

    if (hours > 6 && hours < 19) {
        this->is_night = false;
        this->color_from_sky = this->color_day;
        this->ambient_light = this->day_ambient_light;
    } else {
        this->is_night = true;
        this->color_from_sky = this->color_night;
        this->ambient_light = this->night_ambient_light;
    }

    this->delta_si_real_elapsed = (60 * hours) + minutes;
    this->ambient_next_light = this->ambient_light;
}