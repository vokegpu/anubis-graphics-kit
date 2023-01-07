#ifndef AGK_API_WORLD_MODEL_RENDERER_H
#define AGK_API_WORLD_MODEL_RENDERER_H

#include "api/world/model/model.hpp"
#include "api/mesh/mesh.hpp"
#include <vector>
#include <map>

class renderer : public feature {
protected:
    std::vector<model*> loaded_model_list {};
    std::map<std::string, int32_t> model_register_map {};

    int32_t loaded_light_size {};
    void on_event_refresh_draw(SDL_Event &sdl_event);
public:
    std::vector<world_feature*> wf_draw_list {};

    model *add(std::string_view tag, mesh::data &mesh_data);
    bool find(std::string_view tag, model *& p_model);
    bool contains(std::string_view tag);

    void process_terrain();
    void process_environment();

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_render() override;
};

#endif