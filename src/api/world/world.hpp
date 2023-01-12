#ifndef AGK_API_WORLD_H
#define AGK_API_WORLD_H

#include "api/world/world.hpp"
#include "world_feature.hpp"
#include "api/world/enums/enums.hpp"
#include "api/world/terrain/chunk.hpp"
#include "api/value/value.hpp"
#include "api/util/env.hpp"
#include <vector>
#include <queue>
#include <map>

static const float agk_perspective_clip_distance {1000.0f};

class world : public feature {
protected:
	int32_t wf_chunk_token_id {};
    bool poll_low_priority_queue {};

    void on_event_changed_priority(SDL_Event &sdl_event);
    void do_update_chunk();
    void update_rendering_state(std::string_view chunk_tag, bool flag);
public:
    explicit world();
    ~world();

    /* Start of environment segment. */
	std::map<int32_t, world_feature*> registered_wf_map {};
    std::queue<world_feature*> wf_low_priority_queue {};

    std::vector<world_feature*> wf_list {};
	std::vector<world_feature*> wf_high_priority_list {};

	void registry_wf(world_feature *p_world_feature);
    world_feature *unregister_wf(world_feature *p_world_feature);
    world_feature *find_env_wf(int32_t wf_id);
    /* End of environment segment. */

    /* Start of terrain segment. */
    std::map<std::string, chunk*> chunk_map {};
    std::vector<chunk*> loaded_chunk_list {};

    uint32_t chunk_heightmap_gl_texture {};
    util::texture chunk_heightmap_texture {};
    util::timing chunk_checker_timing {};
    mesh::data chunk_mesh_data {};

    value<int32_t> config_chunk_gen_dist {};
    value<int32_t> config_chunk_size {};
    value<uint64_t> config_chunk_gen_interval {};

    chunk *find_chunk_wf(int32_t wf_id);
    chunk *find_chunk_wf(const std::string &grid_pos);
    /* End of terrain segment. */

    void on_create() override;
    void on_destroy() override;
	void on_update() override;
	void on_event(SDL_Event &sdl_event) override;
};

#endif