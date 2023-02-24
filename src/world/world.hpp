#ifndef AGK_WORLD_H
#define AGK_WORLD_H

#include "world.hpp"
#include "world/environment/object.hpp"
#include "world/enums/enums.hpp"
#include "world/terrain/chunk.hpp"
#include "core/value.hpp"
#include "util/env.hpp"
#include "util/file.hpp"
#include "world/environment/object.hpp"
#include <vector>
#include <queue>
#include <map>

static float agk_perspective_clip_distance {8096.0f * 8096.0f};

class world : public imodule {
protected:
    int32_t chunk_token_id {};
    bool poll_low_priority_queue {};
    uint16_t free_memory_counter {};

    material *p_material_tree_global {};
    std::vector<float> vegetation_memory_list {};

    void on_event_changed_priority(SDL_Event &sdl_event);
    void on_event_refresh_environment_lighting(SDL_Event &sdl_event);
public:
    explicit world() = default;
    ~world() = default;

    /* Start of environment segment. */
    std::map<int32_t, object*> obj_register_map {};
    std::queue<object*> obj_low_priority_queue {};

    std::vector<object*> obj_list {};
    std::vector<object*> obj_high_priority_list {};
    std::vector<int32_t> obj_id_light_list {};

    void registry(object *p_object);
    object *erase(object *p_object);
    object *&find_object(int32_t obj_id);
    /* End of environment segment. */

    paralleling parallel {};
    texturing texture_vegetation {};

    /* Start of terrain segment. */
    std::map<std::string, chunk*> chunk_map {};
    std::vector<chunk*> loaded_chunk_list {};

    util::timing chunk_checker_timing {};
    util::timing chunk_poll_chunking {};
    mesh::data chunk_mesh_data {};
    texturing texture_chunk {};

    std::vector<glm::vec3> near_chunk_global_uv {};
    std::queue<chunk*> queue_chunking {};

    chunk *find_chunk(int32_t wf_id);
    chunk *find_chunk(const std::string &grid_pos);
    /* End of terrain segment. */

    /* Start of world sky segment. */
    util::timing sky_cloud_timing {};
    texturing sky_cloud_texture {};
    std::vector<::mesh::data> sky_cloud_mesh {};
    /* End of world sky segment. */

    void on_create() override;
    void on_destroy() override;
    void on_update() override;
    void on_event(SDL_Event &sdl_event) override;
};

#endif