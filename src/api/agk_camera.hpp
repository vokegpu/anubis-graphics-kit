#pragma once

#ifndef AGK_CAMERA_H
#define AGK_CAMERA_H

#include "api/agk_feature.hpp"
#include <glm/glm.hpp>

class agk_camera : public agk_feature {
protected:
    float previous_mx = 0;
    float previous_my = 0;
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;

    float yaw = 0.0f;
    float pitch = 0.0f;

    float fov;
    float mouse_sensitivity;
    bool mouse_locked;

    agk_camera();
    ~agk_camera();

    glm::mat4 matrix();

    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif