#include "scene_main.hpp"
#include <gl/glew.h>
#include "agk.hpp"
#include "api/agk_util.hpp"

void scene_main::on_start() {
    agk_scene::on_start();
    agk::core->camera->mouse_locked = true;

    glEnable(GL_DEPTH_TEST);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowGrab(agk::core->get_sdl_win(), SDL_FALSE);

    agk::core->camera->position = glm::vec3(0.0f, 0.0f, -2.0f);

    float vert_list[180] = {
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    agk_mesh_stream m_stream;
    m_stream.dispatch(180, vert_list);
    m_stream.dispatch_normal(0.0f, 0.0f, -1.0f);
    m_stream.dispatch_normal(0.0f, 0.0f, 1.0f);
    m_stream.dispatch_normal(-1.0f, 0.0f, 0.0f);
    m_stream.dispatch_normal(0.0f, 1.0f, 0.0f);
    m_stream.dispatch_normal(0.0f, -1.0f, 0.0f);
    m_stream.dispatch_normal(0.0f, 0.0f, 1.0f);

    batch.invoke();
    batch.dispatch(m_stream);
    batch.revoke();
}

void scene_main::on_end() {
    agk_scene::on_end();
}

void scene_main::on_event(SDL_Event &sdl_event) {
    agk_scene::on_event(sdl_event);

    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            agk::core->camera->on_event(sdl_event);
            break;
        }
    }
}

void scene_main::on_update() {
    agk_scene::on_update();
    float v = 0.0f;

    float forward = 0.0f;
    float strafe = 0.0f;
    float fly = 0.0f;

    if (agk_util::pressed[SDLK_w]) {
        forward = 1;
    }

    if (agk_util::pressed[SDLK_s]) {
        forward = -1;
    }

    if (agk_util::pressed[SDLK_a]) {
        strafe = 1;
    }

    if (agk_util::pressed[SDLK_d]) {
        strafe = -1;
    }

    if (agk_util::pressed[SDLK_SPACE]) {
        fly = -1;
    }

    float f = agk::core->camera->yaw;
    float x = glm::cos(glm::radians(f));
    float z = glm::sin(glm::radians(f));
    float y = glm::sin(glm::radians(agk::core->camera->pitch));

    v = 0.2867f;

    this->velocity.x = forward * v * x + strafe * v * z;
    this->velocity.z = forward * v * z - strafe * v * x;
    this->velocity.y = (forward + fly) * v * y;

    agk::core->camera->position += this->velocity * agk_clock::delta_time;
}

void scene_main::on_render() {
    agk_scene::on_render();

    this->batch.draw(glm::vec3(0, 0, 0), glm::vec4(0.3f, 0.4f, 0.5f, 1.0f));
}
