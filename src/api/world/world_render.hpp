#ifndef AGK_API_WORLD_RENDER_H
#define AGK_API_WORLD_RENDER_H

#include "api/gpu/shading_access.hpp"
#include "api/feature/feature.hpp"
#include "api/gpu/buffer_builder.hpp"
#include <glm/glm.hpp>
#include "api/world/material/material.hpp"

class world_render : public feature {
public:
    shading::program object_model_shading {};
    shading::program entity_model_shading {};
    glm::mat4 matrix_perspective {};

    std::map<std::string, int32_t> registered_models_map {};
    std::vector<buffer_builder*> loaded_model_list {};
    material::data undefined_material {};

    buffer_builder* gen_model(const char*);
    buffer_builder* get_model_by_tag(std::string_view);
    int32_t get_model_id_by_tag(std::string_view);
    void update_perspective_matrix();

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event&) override;
    void on_update() override;
    void on_render() override;
};

#endif