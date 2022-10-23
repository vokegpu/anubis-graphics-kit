#ifndef AGK_API_WORLD_RENDER_H
#define AGK_API_WORLD_RENDER_H

#include "api/renderer/shading_access.hpp"
#include "api/feature/feature.hpp"
#include "api/renderer/buffer_builder.hpp"
#include <glm/glm.hpp>

class world_render : public feature {
public:
    shading::program object_model_shading {};
    shading::program entity_model_shading {};

    std::vector<buffer_builder*> loaded_model_list {};
    glm::mat4 matrix_perspective {};

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif