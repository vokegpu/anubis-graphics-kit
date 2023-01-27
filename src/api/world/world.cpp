#include "world.hpp"
#include "api/api.hpp"
#include "api/event/event.hpp"
#include <glm/gtc/matrix_inverse.hpp>
#include "api/util/env.hpp"
#include "api/util/math.hpp"
#include "api/util/file.hpp"
#include "api/util/gpu.hpp"

void world::on_create() {
    this->config_chunk_gen_dist.set_value(3);
    this->config_chunk_gen_interval.set_value(1000);
    this->config_chunk_size.set_value(128 * 4);

    shading::program *p_program_hmap_randomizer {new shading::program {}};
    api::shading::create_program("hmap.randomizer.script", p_program_hmap_randomizer, {
            {"./data/scripts/hmap.randomizer.script.comp", shading::stage::compute}
    });

    this->parallel_chunk.p_program_parallel = p_program_hmap_randomizer;

    p_program_hmap_randomizer->invoke();
    p_program_hmap_randomizer->set_uniform_int("ChunkActiveTexture[0]", 1); // left
    p_program_hmap_randomizer->set_uniform_int("ChunkActiveTexture[1]", 2); // right
    p_program_hmap_randomizer->set_uniform_int("ChunkActiveTexture[2]", 3); // top
    p_program_hmap_randomizer->set_uniform_int("ChunkActiveTexture[3]", 4); // bottom
    p_program_hmap_randomizer->revoke();

    util::read_image("./data/textures/iceland_heightmap.png", this->chunk_heightmap_texture);
    api::mesh::loader().load_identity_heightmap(this->chunk_mesh_data, 20, 20);

    this->texture_chunk.invoke(0, {GL_TEXTURE_2D, GL_UNSIGNED_BYTE});
    this->texture_chunk.send<uint8_t>({128, 128, 0}, this->chunk_heightmap_texture.p_data, {GL_RGBA, GL_RGBA});
    this->texture_chunk.revoke();
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
    if (util::reset_when(this->chunk_checker_timing, this->config_chunk_gen_interval.get_value())) {
        this->do_update_chunk();
    } else if (!this->queue_chunking.empty() && util::reset_when(this->chunk_poll_chunking, 16)) {
        chunk *&p_chunk {this->queue_chunking.front()};

        /* Invoke parallel computation to randomize map. */
        this->parallel_chunk.invoke();
        this->parallel_chunk.memory_barrier =  GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;

        this->parallel_chunk.p_program_parallel->set_uniform_float("Delta", static_cast<float>(SDL_GetTicks())  / 1000);
        this->parallel_chunk.p_program_parallel->set_uniform_float("Frequency", this->config_chunk_frequency.get_value());
        this->parallel_chunk.p_program_parallel->set_uniform_float("Amplitude", this->config_chunk_amplitude.get_value());
        this->parallel_chunk.p_program_parallel->set_uniform_float("Persistence", this->config_chunk_persistence.get_value());
        this->parallel_chunk.p_program_parallel->set_uniform_float("Lacunarity", this->config_chunk_lacunarity.get_value());
        this->parallel_chunk.p_program_parallel->set_uniform_int("Octaves", this->config_chunk_octaves.get_value());

        int32_t chunk_size {this->config_chunk_size.get_value()};
        float chunk_resolution[2] {static_cast<float>(chunk_size), static_cast<float>(chunk_size)};

        std::string grid_next {};
        glm::ivec2 player_grid {};
        glm::ivec4 iteration {};
        uint32_t texture_pass {};

        bool contains_chunk {};
        auto &p_player {api::world::current_player()};

        player_grid.x = static_cast<int32_t>(p_player->position.x);
        player_grid.y = static_cast<int32_t>(p_player->position.z);
        util::to_grid_pos(player_grid, p_player->position, glm::ivec2(chunk_size));

        for (int it {}; it < 4; it++) {
            iteration.x = player_grid.x + util::SURROUND[iteration.y++];
            iteration.z = player_grid.y + util::SURROUND[iteration.y++];

            grid_next.clear();
            grid_next += std::to_string(iteration.x);
            grid_next += 'x';
            grid_next += std::to_string(iteration.y);

            auto &chunk {this->chunk_map[grid_next]};
            contains_chunk = chunk != nullptr;
            
            if (contains_chunk) {
                glActiveTexture(GL_TEXTURE0 + iteration.w);
                glBindTexture(GL_TEXTURE_2D, chunk->texture);
            }

            this->parallel_chunk.p_program_parallel->set_uniform_bool("ChunkContains[" + std::to_string(iteration.w) + ']',  contains_chunk);
            iteration.w++;
        }

        this->parallel_chunk.dimension[0] = chunk_size;
        this->parallel_chunk.dimension[1] = chunk_size;
        this->parallel_chunk.dispatch_groups[0] = 32;
        this->parallel_chunk.dispatch_groups[1] = 32;

        uint32_t texture_key {static_cast<uint32_t>(p_chunk->id)};
        if (!this->queue_texture_trash.empty()) {
            texture_key = this->queue_texture_trash.front();
            this->queue_texture_trash.pop();

            if (this->queue_texture_trash.size() >= 12) {
                for (;!this->queue_texture_trash.empty();) {
                    glDeleteTextures(1, &this->queue_texture_trash.front());
                    this->queue_texture_trash.pop();
                }
            }
        }

        this->texture_chunk.invoke(texture_key, {GL_TEXTURE_2D, GL_FLOAT});
        this->texture_chunk.send<float>({chunk_size, chunk_size, 0}, nullptr, {GL_RGBA32F, GL_RGBA});
        this->parallel_chunk.attach(0, this->texture_chunk[p_chunk->id], GL_WRITE_ONLY);
        this->parallel_chunk.dispatch();

        p_chunk->texture = this->texture_chunk[p_chunk->id].id;
        this->loaded_chunk_list.push_back(p_chunk);
        this->config_delta.set_value(this->config_delta.get_value() + 1.0f);
        api::world::renderer()->add(p_chunk);
        
        this->queue_chunking.pop();
        this->parallel_chunk.revoke();
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
    glm::vec3 sub {};
    auto &p_player {api::world::current_player()};
    std::vector<chunk*> current_loaded_chunk {};

    int32_t chunk_size {this->config_chunk_size.get_value()};
    int32_t chunk_gen_dist {this->config_chunk_gen_dist.get_value()};

    glm::ivec2 grid_pos {};
    glm::ivec2 vec_chunk_size {chunk_size, chunk_size};

    for (chunk *&p_chunks : this->loaded_chunk_list) {
        sub = p_chunks->position + (float) chunk_size / 2;
        sub = p_player->position - sub;
        sub.y = 0;

        util::to_grid_pos(grid_pos, p_chunks->position, vec_chunk_size);
        std::string chunk_tag {std::to_string(grid_pos.x)};
        chunk_tag += 'x';
        chunk_tag += std::to_string(grid_pos.y);

        if (static_cast<int32_t>(glm::length(sub)) / chunk_size > chunk_gen_dist * chunk_gen_dist) {
            api::world::renderer()->refresh();
            this->queue_texture_trash.push(p_chunks->texture);
            p_chunks->on_destroy();
            delete p_chunks;
            p_chunks = nullptr;
            this->chunk_map.erase(chunk_tag);
            continue;
        }

        current_loaded_chunk.push_back(p_chunks);
        this->chunk_map[chunk_tag] = p_chunks;
    }

    this->loaded_chunk_list = current_loaded_chunk;

    glm::ivec2 player_grid {};
    glm::vec3 chunk_scale {6.4f * 4, 1.0f, 6.4f * 4};

    player_grid.x = static_cast<int32_t>(p_player->position.x);
    player_grid.y = static_cast<int32_t>(p_player->position.z);
    util::to_grid_pos(player_grid, p_player->position, glm::ivec2(chunk_size));

    for (int32_t z {-chunk_gen_dist}; z < chunk_gen_dist; ++z) {
        for (int32_t x {-chunk_gen_dist}; x < chunk_gen_dist; ++x) {
            grid_pos.x = player_grid.x + x;
            grid_pos.y = player_grid.y + z;

            std::string chunk_tag {std::to_string(grid_pos.x)};
            chunk_tag += 'x';
            chunk_tag += std::to_string(grid_pos.y);

            if (this->chunk_map[chunk_tag] != nullptr) {
                continue;
            }

            chunk *p_chunk {new chunk {}};
            p_chunk->on_create();
            p_chunk->position.x = grid_pos.x * chunk_size;
            p_chunk->position.z = grid_pos.y * chunk_size;
            p_chunk->id = (int32_t) ++this->wf_chunk_token_id;
            p_chunk->scale = chunk_scale;

            this->queue_chunking.push(p_chunk);
            this->chunk_map[chunk_tag] = p_chunk;
        }
    }
}

chunk *world::find_chunk_wf(int32_t wf_id) {
    for (chunk *&p_chunk : this->loaded_chunk_list) {
        if (p_chunk != nullptr && p_chunk->id == wf_id) {
            return p_chunk;
        }
    }

    return nullptr;
}

chunk *world::find_chunk_wf(const std::string &grid_pos) {
    return grid_pos.empty() ? nullptr : this->chunk_map[grid_pos.data()];
}

void world::gen_chunk(std::string &chunk_tag, const glm::ivec3 &ipos, const glm::vec3 &scale) {

}