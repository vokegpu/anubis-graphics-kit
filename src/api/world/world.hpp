#ifndef AGK_API_WORLD_H
#define AGK_API_WORLD_H

#include "api/world/world.hpp"
#include "world_feature.hpp"
#include "api/world/enums/enums.hpp"
#include <vector>
#include <queue>
#include <map>

class world : public feature {
protected:
	int32_t wf_token_id {};
public:
	std::map<int32_t, world_feature*> registered_wf_map {};
	std::vector<world_feature*> wf_list {}, wf_draw_list {};
	std::queue<world_feature*> wf_process_queue {};

	void registry_wf(world_feature *wf);
	void refresh_wf_renderable();
	void append_process(world_feature *wf);

	void on_create() override;
	void on_destroy() override;
	void on_update() override;
	void on_event(SDL_Event &sdl_event) override;
	void on_render() override;
};

#endif