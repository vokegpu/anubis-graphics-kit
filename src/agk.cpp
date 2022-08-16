#include "agk.hpp"
#include <iostream>

agk_client* const agk::core = new agk_client();

int main(int argv, char** argc) {
    agk::core->init();
    agk::core->run();
    agk::core->shutdown();

    return 0;
}

void agk::push_back_camera(gpu::program &program) {
    glm::mat4 matrix_camera_view = agk::core->camera->matrix();
    glm::mat4 matrix_perspective = agk::core->matrix_perspective();

    program.setm4f("CameraViewMatrix", &matrix_camera_view[0][0]);
    program.setm4f("ProjectionMatrix", &matrix_perspective[0][0]);
}
