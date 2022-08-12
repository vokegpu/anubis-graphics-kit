#include "scene_cube.hpp"
#include <gl/glew.h>
#include "agk.hpp"
#include "api/agk_renderer_manager.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <api/agk_util.hpp>

void scene_cube::on_start() {
    agk_scene::on_start();
    the_agk_core->camera->mouse_locked = true;

    GLfloat vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    GLuint indices[] = {
            0, 1, 2,
            3, 4, 5
    };

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*) (sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void scene_cube::on_end() {
    agk_scene::on_end();
}

void scene_cube::on_event(SDL_Event &sdl_event) {
    agk_scene::on_event(sdl_event);

    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            the_agk_core->camera->on_event(sdl_event);
            break;
        }
    }
}

void scene_cube::on_update() {
    agk_scene::on_update();

    if (util::pressed[SDLK_w]) {
        this->velocity.z = 1.0f;
    }

    if (util::pressed[SDLK_s]) {
        this->velocity.z = -1.0f;
    }

    if (util::pressed[SDLK_a]) {
        this->velocity.x = 1.0f;
    }

    if (util::pressed[SDLK_d]) {
        this->velocity.x = -1.0f;
    }

    float f = glm::length(this->velocity);
    this->velocity.x += glm::sin(f) * 0.148f;
    this->velocity.z -= glm::cos(f) * 0.148f;

    the_agk_core->camera->position += this->velocity * agk_clock::delta_time;
    this->velocity *= 0.1f;
}

void scene_cube::on_render() {
    agk_scene::on_render();

    glm::mat4 projection = glm::perspective(glm::radians(the_agk_core->camera->fov), the_agk_core->get_screen_width() / the_agk_core->get_screen_height(), 0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0, 0, 0));
    float color[] = {0.0f, 0.0f, 0.6f, 1.0f};

    agk_fx_manager::fx_model.use();
    agk_fx_manager::fx_model.set4fm("u_mat4_perspective", &projection[0][0]);
    agk_fx_manager::fx_model.set4fm("u_mat4_view", &the_agk_core->camera->matrix()[0][0]);
    agk_fx_manager::fx_model.set4fm("u_mat4_model", &model[0][0]);
    agk_fx_manager::fx_model.set4f("u_vec4_color", color);

    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
