#ifndef AGK_API_WORLD_H
#define AGK_API_WORLD_H

#include "api/world/world.hpp"
#include "world_feature.hpp"
#include "api/world/enums/enums.hpp"
#include "api/world/terrain/chunk.hpp"
#include <vector>
#include <queue>
#include <map>

static const float agk_perspective_clip_distance {1000.0f};

class world : public feature {
protected:
	int32_t wf_token_id {};
    bool poll_low_priority_queue {};

    void on_event_refresh_priority(SDL_Event &sdl_event);
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
    world_feature *find(int32_t wf_id);
    /* End of environment segment. */

    /* Start of terrain segment. */
    std::vector<chunk*> loaded_chunk_list {};
    util::timing chunk_checker_timing {};
    /* End of terrain segment. */

    void on_create() override;
    void on_destroy() override;
	void on_update() override;
	void on_event(SDL_Event &sdl_event) override;
};

#endif