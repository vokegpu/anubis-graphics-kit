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

#include "asset/mesh.hpp"
#include "world/terrain/chunk.hpp"
#include "core/value.hpp"
#include "ui/immshape.hpp"
#include <vector>
#include <map>
#include <array>

class renderer : public imodule {
protected:
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
    std::vector<object*> wf_env_draw_list {};
    std::vector<chunk*> wf_chunk_draw_list {};

    void add(chunk *p_chunk);
    void refresh();

    void process_terrain();
    void process_environment();
    void process_post_processing();
    void process_editor();
    void process_sky();
    void process_framebuffer(int32_t w, int32_t h);

    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_render() override;
};

#endif