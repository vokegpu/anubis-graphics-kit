#include "world.hpp"
#include "api/api.hpp"
#include "api/event/event.hpp"
#include <glm/gtc/matrix_inverse.hpp>
#include "api/util/env.hpp"
#include "api/util/math.hpp"
#include <lodepng/lodepng.h>

world::world() {
}

world::~world() {
}

void world::on_create() {
    this->chunk_size.set_value(512);
    this->chunk_check_delay.set_value(1000);

    this->chunk_heightmap_texture.path = "./data/textures/rolling_hills_heightmap.png";
    util::loadtexture(&this->chunk_heightmap_texture);
}

void world::on_destroy() {
}

void world::registry_wf(world_feature *p_world_feature) {
	if (p_world_feature == nullptr) {
		return;
	}

    if (p_world_feature->id == 0) p_world_feature->id = feature::token++;
	api::gc::create(p_world_feature);

	this->registered_wf_map[p_world_feature->id] = p_world_feature;
	this->wf_list.push_back(p_world_feature);
}

void world::on_event(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            api::app.p_current_camera->on_event(sdl_event);
            break;
        }

        case SDL_USEREVENT: {
            switch (sdl_event.user.code) {
                case event::WORLD_CHANGED_PRIORITY: {
                    this->on_event_changed_priority(sdl_event);
                    break;
                }
            }

            break;
        }
    }
}

void world::on_update() {
    /* World feature environment segment. */
    if (this->poll_low_priority_queue) {
        for (;!this->wf_low_priority_queue.empty();) {
            auto &p_world_feature {this->wf_low_priority_queue.front()};
            if (p_world_feature != nullptr) p_world_feature->on_low_update();
            this->wf_low_priority_queue.pop();
        }
    }

    // todo repeated high priority wf(s)
    for (world_feature *&p_world_feature : this->wf_high_priority_list) {
        if (p_world_feature != nullptr) {
            p_world_feature->on_update();
        }
    }

    /* World terrain segment. */
    if (util::resetifreach(this->chunk_checker_timing, this->chunk_check_delay.get_value())) {
        this->do_update_chunk();
    }
}

world_feature *world::unregister_wf(world_feature *p_world_feature) {
    if (p_world_feature != nullptr) {
        this->registered_wf_map.erase(p_world_feature->id);
        api::gc::destroy(p_world_feature);
        return p_world_feature;
    }

    // todo: unregister object from world.
    return nullptr;
}

world_feature *world::find_env_wf(int32_t wf_id) {
    return this->registered_wf_map[wf_id];
}

void world::on_event_changed_priority(SDL_Event &sdl_event) {
    auto *p_high {static_cast<bool*>(sdl_event.user.data1)};
    auto *p_wf_id {static_cast<int32_t*>(sdl_event.user.data2)};

    world_feature *p_world_feature_target {this->find_env_wf(*p_wf_id)};
    if (p_world_feature_target != nullptr && !p_high && p_world_feature_target->get_priority() != enums::priority::low) {
        p_world_feature_target->set_priority(enums::priority::low, false);
        this->wf_high_priority_list.clear();

        for (world_feature *&p_world_feature : this->wf_list) {
            if (p_world_feature != nullptr && p_world_feature->get_priority() == enums::priority::high) {
                this->wf_high_priority_list.push_back(p_world_feature);
            }
        }
    } else if (p_world_feature_target != nullptr && p_high && p_world_feature_target->get_priority() != enums::priority::high) {
        p_world_feature_target->set_priority(enums::priority::high, false);
        this->wf_high_priority_list.push_back(p_world_feature_target);
    }

    if (p_world_feature_target != nullptr && p_world_feature_target->get_priority() == enums::priority::low) {
        this->wf_low_priority_queue.push(p_world_feature_target);
        this->poll_low_priority_queue = true;
    }

    delete p_wf_id;
    delete p_high;
}

void world::do_update_chunk() {
    if (!this->loaded_chunk_list.empty()) {
        return;
    }

    chunk *p_chunk {new chunk {}};
    p_chunk->position = {0, 0, 0};
    p_chunk->id = (int32_t) ++this->wf_chunk_token_id;
    p_chunk->gen_chunk(nullptr, this->chunk_size.get_value(), this->chunk_size.get_value());

    glm::ivec2 grid_pos {};
    util::transform_to_grid_pos(grid_pos, p_chunk->position, glm::ivec2(this->chunk_size.get_value()));
    const std::string chunk_tag {std::to_string(grid_pos.x) + "x" + std::to_string(grid_pos.y)};

    this->chunk_map[chunk_tag] = p_chunk;
    this->loaded_chunk_list.push_back(p_chunk);

    SDL_Event sdl_event_chunk {};
    sdl_event_chunk.user.data1 = new std::string(chunk_tag);
    sdl_event_chunk.user.data2 = new bool(true); // add chunk flag
    event::dispatch(sdl_event_chunk, event::WORLD_REFRESH_CHUNK);
}

chunk *world::find_chunk_wf(int32_t wf_id) {
    for (chunk *&p_chunk : this->loaded_chunk_list) {
        if (p_chunk != nullptr && p_chunk->id == wf_id) {
            return p_chunk;
        }
    }

    return nullptr;
}

chunk *world::find_chunk_wf(std::string_view grid_pos) {
    return this->chunk_map[grid_pos.data()];
}
