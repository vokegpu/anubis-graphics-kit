#ifndef AGK_API_WORLD_MODEL_RENDERER_H
#define AGK_API_WORLD_MODEL_RENDERER_H

#include "api/world/model/model.hpp"
#include "api/mesh/mesh.hpp"
#include "api/world/terrain/chunk.hpp"
#include "api/value/value.hpp"
#include "api/ui/immshape.hpp"
#include <vector>
#include <map>

class renderer : public feature {
protected:
    std::vector<model*> loaded_model_list {};
    std::map<std::string, int32_t> model_register_map {};

    int32_t loaded_light_size {};
    glm::mat4 mat4x4_mvp {};
    bool update_disabled_chunks {};

    buffering buffer_post_processing {};
    immshape immshape_post_processing {};
    std::map<std::string, framebuffering> framebuffer_map {};
    paralleling<float> high_resolution_hdr {};

    void on_event_refresh_environment(SDL_Event &sdl_event);
    void on_event_refresh_chunk(SDL_Event &sdl_event);
public:
    std::vector<world_feature*> wf_env_draw_list {};
    std::vector<chunk*> wf_chunk_draw_list {};

    value<glm::vec2> config_fog_distance {};
    value<glm::vec3> config_fog_color {};
    value<bool> config_post_processing {};
    value<bool> config_hdr {};

    model *add(std::string_view tag, mesh::data &mesh_data);
    void add(chunk *p_chunk);

    bool find(std::string_view tag, model *& p_model);
    bool contains(std::string_view tag);
    void refresh();

    void process_terrain();
    void process_environment();
    void process_post_processing();

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_render() override;
};

#endif