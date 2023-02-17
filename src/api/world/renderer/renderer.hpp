#ifndef AGK_API_WORLD_MODEL_RENDERER_NODE_H
#define AGK_API_WORLD_MODEL_RENDERER_NODE_H

#include <iostream>

typedef struct node {
    int parent {};
    int id {};
    std::string model {};
} node;

#endif

#ifndef AGK_API_WORLD_MODEL_RENDERER_H
#define AGK_API_WORLD_MODEL_RENDERER_H

#include "api/world/model/model.hpp"
#include "api/mesh/mesh.hpp"
#include "api/world/terrain/chunk.hpp"
#include "api/value/value.hpp"
#include "api/ui/immshape.hpp"
#include <vector>
#include <map>
#include <array>

class renderer : public feature {
protected:
    std::vector<model*> loaded_model_list {};
    std::map<std::string, int32_t> model_register_map {};

    std::vector<node> instanced_node_list {};
    std::map<int32_t, ::mesh::data> instanced_mesh_map {};

    std::vector<float> instance_mat4x4 {};
    std::vector<float> instance_mat3x3 {};

    glm::mat4 mat4x4_perspective_view {};
    glm::mat4 mat4x4_inverse_perspective_view {};
    glm::mat4 mat4x4_previous_perspective_view {};
    glm::mat4 mat4x4_view {};
    glm::mat4 mat4x4_perspective {};

    int32_t loaded_light_size {};
    bool update_disabled_chunks {};
    util::timing timing_instances {};

    /* Chunking. */
    buffering buffer_chunk {};
    texturing texture_chunk {};

    /* Post-processing. */
    buffering buffer_post_processing {};
    immshape immshape_post_processing {};
    paralleling parallel_post_processing {};
    texturing texture_post_processing {};
    framebuffering framebuffer_post_processing {};
    util::timing timing_hdr_cycle {};
    glm::vec2 camera_motion_delta {};

    /* Debug. */
    buffering buffer_coordinate_debug {};

    void on_event_refresh_environment(SDL_Event &sdl_event);
    void on_event_refresh_chunk(SDL_Event &sdl_event);
public:
    std::vector<world_feature*> wf_env_draw_list {};
    std::vector<chunk*> wf_chunk_draw_list {};

    model *add(std::string_view tag, mesh::data &mesh_data);
    void add(chunk *p_chunk);

    bool find(std::string_view tag, model *& p_model);
    bool contains(std::string_view tag);
    void refresh();

    ::mesh::data &interact_with_instanced_mesh(int32_t id);
    int32_t add_mesh_instance(int32_t id, uint32_t key, buffering &buffer, const glm::mat4 &matrix);
    int32_t remove_mesh_instance(int32_t id, int32_t instance);

    void process_terrain();
    void process_environment();
    void process_post_processing();
    void process_editor();
    void process_sky();
    void process_framebuffer(int32_t w, int32_t h);
    void process_instances();

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_render() override;
};

#endif