#ifndef AGK_API_WORLD_CAMERA_H
#define AGK_API_WORLD_CAMERA_H

#include <glm/glm.hpp>
#include "api/feature/feature.hpp"

class camera3d : public feature {
public:
    glm::vec3 position {};
    glm::vec3 front {};
    glm::vec3 right {};
    glm::vec3 up {};
    glm::vec3 world_up {};

    float yaw {};
    float pitch {};
    float field_of_view {90.0f};
    float sensitivity {0.2f};
    bool enabled {true};

    glm::mat4 get_matrix_camera_view();
    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
};

#endif