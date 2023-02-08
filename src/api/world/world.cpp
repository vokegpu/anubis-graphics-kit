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
    this->near_chunk_global_uv.reserve(4);

    shading::program *p_program_hmap_randomizer {new shading::program {}};
    api::shading::create_program("hmap.randomizer.script", p_program_hmap_randomizer, {
            {"./data/scripts/hmap.randomizer.script.comp", shading::stage::compute}
    });

    this->parallel_chunk.p_program_parallel = p_program_hmap_randomizer;

    util::read_image("./data/textures/iceland_heightmap.png", this->chunk_heightmap_texture);
    api::mesh::loader().load_identity_heightmap(this->chunk_mesh_data, 20, 20);
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

        /* Use grid position as global UV for height map connection. */
        glm::vec2 global_uv {};
        glm::ivec2 grid_pos {};
        glm::ivec2 global_uv_grid {};

        int32_t chunk_size {this->config_chunk_size.get_value()};
        util::to_grid_pos(grid_pos, p_chunk->position, {chunk_size, chunk_size});
        global_uv = grid_pos;

        /* Invoke parallel computation to randomize map. */
        this->parallel_chunk.invoke();
        this->parallel_chunk.memory_barrier = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;

        this->parallel_chunk.p_program_parallel->set_uniform_float("uFrequency", p_chunk->metadata.frequency);
        this->parallel_chunk.p_program_parallel->set_uniform_float("uAmplitude", p_chunk->metadata.amplitude);
        this->parallel_chunk.p_program_parallel->set_uniform_float("uPersistence", p_chunk->metadata.persistence);
        this->parallel_chunk.p_program_parallel->set_uniform_float("uLacunarity", p_chunk->metadata.lacunarity);
        this->parallel_chunk.p_program_parallel->set_uniform_int("uOctaves", p_chunk->metadata.octaves);
        this->parallel_chunk.p_program_parallel->set_uniform_vec2("uGlobalUV", &global_uv[0]);

        /* Read nearest chunk metadata. */
        int32_t near_chunks {};
        int32_t tick {};

        bool should_check_into_shader {};
        float chunk_seedf[4] {};

        for (int32_t it {}; it < 4; it++) {
            global_uv_grid.x = (grid_pos.x + util::SURROUND[near_chunks++]);
            global_uv_grid.y = (grid_pos.y + util::SURROUND[near_chunks++]);

            std::string chunk_tag {std::to_string(global_uv_grid.x)};
            chunk_tag += 'x';
            chunk_tag += std::to_string(global_uv_grid.y);

            glm::vec3 &near {this->near_chunk_global_uv[it]};
            near.x = static_cast<float>(global_uv_grid.x);
            near.y = static_cast<float>(global_uv_grid.y);
            near.z = -1;

            /* Check if the metadata is different to infuse together. */
            auto &p_near_chunk {this->chunk_map[chunk_tag]};
            if (p_near_chunk != nullptr && p_chunk->is_processed()) {
                chunk_seedf[0] = p_near_chunk->metadata.frequency;
                chunk_seedf[1] = p_near_chunk->metadata.amplitude;
                chunk_seedf[2] = p_near_chunk->metadata.persistence;
                chunk_seedf[3] = p_near_chunk->metadata.lacunarity;
                near.z = 1;

                chunk_tag = "uChunkMetadata[";
                chunk_tag += std::to_string(tick);
                chunk_tag += ']';

                this->parallel_chunk.p_program_parallel->set_uniform_vec4(chunk_tag + ".uSeedf", chunk_seedf);
                this->parallel_chunk.p_program_parallel->set_uniform_int(chunk_tag + ".uSeedi", p_near_chunk->metadata.octaves);
                if (!should_check_into_shader) should_check_into_shader = p_near_chunk->metadata != p_chunk->metadata;
            }

            chunk_tag = "uChunkMetadata[";
            chunk_tag += std::to_string(tick);
            chunk_tag += ']';
            this->parallel_chunk.p_program_parallel->set_uniform_vec3(chunk_tag + ".uGlobalUV", &near[0]);
            tick++;
        }

        this->parallel_chunk.p_program_parallel->set_uniform_bool("uCheckNearest", should_check_into_shader);

        this->parallel_chunk.dimension[0] = chunk_size;
        this->parallel_chunk.dimension[1] = chunk_size;
        this->parallel_chunk.dispatch_groups[0] = 16;
        this->parallel_chunk.dispatch_groups[1] = 16;

        uint32_t texture_key {static_cast<uint32_t>(p_chunk->id)};
        this->texture_chunk.invoke(texture_key, {GL_TEXTURE_2D, GL_FLOAT});
        this->texture_chunk.send<float>({128, 128, 0}, nullptr, {GL_RGBA32F, GL_RGBA});
        this->parallel_chunk.attach(0, this->texture_chunk[p_chunk->id], GL_WRITE_ONLY);
        this->parallel_chunk.dispatch();

        p_chunk->texture = this->texture_chunk[texture_key].id;

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

    glm::ivec2 player_grid {};
    player_grid.x = static_cast<int32_t>(p_player->position.x);
    player_grid.y = static_cast<int32_t>(p_player->position.z);
    util::to_grid_pos(player_grid, p_player->position, vec_chunk_size);

    float div {static_cast<float>(chunk_size) / 2};
    float distance_toward_generator {(static_cast<float>(chunk_gen_dist * 2) * static_cast<float>(chunk_size))};
    glm::vec3 player_scaled_pos {static_cast<float>(player_grid.x * chunk_size) + div, 0.0f, static_cast<float>(player_grid.y * chunk_size) + div};

    for (chunk *&p_chunks : this->loaded_chunk_list) {
        util::to_grid_pos(grid_pos, p_chunks->position, vec_chunk_size);
        std::string chunk_tag {std::to_string(grid_pos.x)};
        chunk_tag += 'x';
        chunk_tag += std::to_string(grid_pos.y);

        sub = player_scaled_pos - (p_chunks->position + div);
        sub.y = 0.0f;

        if (glm::length(sub) > distance_toward_generator) {
            api::world::renderer()->refresh();
            this->texture_chunk.delete_buffer(static_cast<uint32_t>(p_chunks->id));
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
    float scale_factor {static_cast<float>(this->config_chunk_size.get_value()) / 128};
    scale_factor *= 6.4f;
    glm::vec3 chunk_scale {scale_factor, scale_factor, scale_factor};

    for (int32_t z {-chunk_gen_dist}; z != chunk_gen_dist; z++) {
        for (int32_t x {-chunk_gen_dist}; x != chunk_gen_dist; x++) {
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
            p_chunk->position.x = static_cast<float>(grid_pos.x * chunk_size);
            p_chunk->position.z = static_cast<float>(grid_pos.y * chunk_size);
            p_chunk->id = (int32_t) ++this->wf_chunk_token_id;
            p_chunk->scale = chunk_scale;

            /* Generate a metadata seed to this chunk. */
            p_chunk->metadata = {
                    this->config_chunk_frequency.get_value(),
                    this->config_chunk_amplitude.get_value(),
                    this->config_chunk_persistence.get_value(),
                    this->config_chunk_lacunarity.get_value(),
                    this->config_chunk_octaves.get_value()
            };

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