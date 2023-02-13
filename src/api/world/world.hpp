#ifndef AGK_API_WORLD_H
#define AGK_API_WORLD_H

#include "api/world/world.hpp"
#include "world_feature.hpp"
#include "api/world/enums/enums.hpp"
#include "api/world/terrain/chunk.hpp"
#include "api/value/value.hpp"
#include "api/util/env.hpp"
#include "api/util/file.hpp"
#include "api/world/model/model.hpp"
#include <vector>
#include <queue>
#include <map>

static const float agk_perspective_clip_distance {8096.0f * 8096.0f};

class world : public feature {
protected:
	int32_t wf_chunk_token_id {};
    bool poll_low_priority_queue {};
    uint16_t free_memory_counter {};

    material *p_material_tree {};
    model *p_tree_model {};
    std::vector<float> vegetation_memory_list {};

    void on_event_changed_priority(SDL_Event &sdl_event);
public:
    explicit world() = default;
    ~world() = default;

    /* Start of environment segment. */
	std::map<int32_t, world_feature*> registered_wf_map {};
    std::queue<world_feature*> wf_low_priority_queue {};

    std::vector<world_feature*> wf_list {};
	std::vector<world_feature*> wf_high_priority_list {};

	void registry_wf(world_feature *p_world_feature);
    world_feature *unregister_wf(world_feature *p_world_feature);
    world_feature *&find_env_wf(int32_t wf_id);
    /* End of environment segment. */

    paralleling parallel {};
    texturing texture_vegetation {};

    /* Start of terrain segment. */
    std::map<std::string, chunk*> chunk_map {};
    std::vector<chunk*> loaded_chunk_list {};

    util::image chunk_heightmap_texture {};
    util::timing chunk_checker_timing {};
    util::timing chunk_poll_chunking {};
    mesh::data chunk_mesh_data {};
    texturing texture_chunk {};

    std::vector<glm::vec3> near_chunk_global_uv {};
    std::queue<chunk*> queue_chunking {};

    chunk *find_chunk_wf(int32_t wf_id);
    chunk *find_chunk_wf(const std::string &grid_pos);
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