#ifndef AGK_WORLD_RENDERER_H
#define AGK_WORLD_RENDERER_H

#include "stream/stream.hpp"
#include "world/terrain/chunk.hpp"
#include "core/value.hpp"
#include "ui/shape.hpp"
#include <vector>
#include <map>
#include <array>
#include "gpu/gpuframebuffer.hpp"
#include "gpu/gputexture.hpp"
#include "gpu/gpuparallel.hpp"

class renderer : public imodule {
protected:
    glm::mat4 mat4x4_perspective_view {};
    glm::mat4 mat4x4_inverse_perspective_view {};
    glm::mat4 mat4x4_previous_perspective_view {};
    glm::mat4 mat4x4_view {};
    glm::mat4 mat4x4_perspective {};

    uint64_t loaded_light_size {};
    bool update_disabled_chunks {};

    /* C. */
    buffering buffer_chunk {};
    texturing texture_chunk {};

    /* Post-processing. */
    buffering buffer_post_processing {};
    shape shape_post_processing {};
    paralleling parallel_post_processing {};
    texturing texture_post_processing {};
    framebuffering framebuffer_post_processing {};
    util::timing timing_hdr_cycle {};
    glm::vec2 camera_motion_delta {};

    float previous_average_luminosity {};

    /* Debug. */
    buffering buffer_coordinate_debug {};

    void on_event_refresh_environment(SDL_Event &sdl_event);
public:
    std::vector<object*> obj_draw_list {};
    std::vector<chunk*> chunk_draw_list {};

    buffering buffer_global {};

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