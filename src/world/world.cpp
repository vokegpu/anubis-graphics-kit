#include "world.hpp"
#include "agk.hpp"
#include "util/event.hpp"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "util/env.hpp"
#include "util/math.hpp"
#include "util/file.hpp"
#include "world/environment/object.hpp"

void world::on_create() {
    agk::app.setting.chunk_generation_interval.set_value(1000);
    agk::app.setting.chunk_dimension.set_value(128 * 4);

    agk::mesh::loader().load_identity_heightmap(this->chunk_mesh_data, 20, 20);
    this->vegetation_memory_list.reserve(100 * 4);

    this->p_material_tree = new material {enums::material::dialetric};
    this->p_material_tree->set_color({0.2, 0.0f, 0.2f});

    util::log("Tree simple assets created");

    /* Generate texture for parallel vegetation spawn. */
    this->texture_vegetation.invoke(0, {GL_TEXTURE_2D, GL_FLOAT});
    this->texture_vegetation.send<float>({100, 1, 1}, nullptr, {GL_RGBA16F, GL_RGBA});
    this->texture_vegetation.revoke();
}

void world::on_destroy() {
}

void world::registry_wf(object *p_object) {
	if (p_object == nullptr) {
		return;
	}

    agk::task::synchronize(p_object);
	this->registered_wf_map[p_object->id] = p_object;
	this->wf_list.push_back(p_object);
}

void world::on_event(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            agk::app.p_curr_camera->on_event(sdl_event);
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
    /* World feature environment statement. */
    if (this->poll_low_priority_queue) {
        for (;!this->wf_low_priority_queue.empty();) {
            auto &p_world_feature {this->wf_low_priority_queue.front()};
            if (p_world_feature != nullptr) p_world_feature->on_low_update();
            this->wf_low_priority_queue.pop();
        }
    }

    // @TODO repeated high priority wf(s)
    for (object *&p_world_feature : this->wf_high_priority_list) {
        if (p_world_feature != nullptr) {
            p_world_feature->on_update();
        }
    }

    /* World terrain statement. */
    if (util::reset_when(this->chunk_checker_timing, agk::app.setting.chunk_generation_interval.get_value())) {
        glm::vec3 sub {};
        auto &p_player {agk::world::current_player()};
        std::vector<chunk*> current_loaded_chunk {};

        int32_t chunk_size {agk::app.setting.chunk_dimension.get_value()};
        int32_t chunk_gen_dist {agk::app.setting.chunk_generation_distance.get_value()};

        glm::ivec2 grid_pos {};
        glm::ivec2 vec_chunk_size {chunk_size, chunk_size};

        glm::ivec2 player_grid {};
        player_grid.x = static_cast<int32_t>(p_player->transform.position.x);
        player_grid.y = static_cast<int32_t>(p_player->transform.position.z);
        util::to_grid_pos(player_grid, p_player->transform.position, vec_chunk_size);

        float div {static_cast<float>(chunk_size) / 2};
        float distance_toward_generator {(static_cast<float>(chunk_gen_dist * 2) * static_cast<float>(chunk_size))};
        glm::vec3 player_scaled_pos {static_cast<float>(player_grid.x * chunk_size) + div, 0.0f, static_cast<float>(player_grid.y * chunk_size) + div};
        bool should_refresh_renderer {};

        for (chunk *&p_chunks : this->loaded_chunk_list) {
            util::to_grid_pos(grid_pos, p_chunks->transform.position, vec_chunk_size);
            std::string chunk_tag {std::to_string(grid_pos.x)};

            chunk_tag += 'x';
            chunk_tag += std::to_string(grid_pos.y);

            sub = player_scaled_pos - (p_chunks->transform.position + div);
            sub.y = 0.0f;

            if (glm::length(sub) > distance_toward_generator) {
                this->texture_chunk.delete_buffer(static_cast<uint32_t>(p_chunks->id));
                p_chunks->on_destroy();
                should_refresh_renderer = true;

                for (int32_t &id : p_chunks->vegetation) {
                    this->unregister_wf(this->registered_wf_map[id]);
                    this->registered_wf_map.erase(id);
                    free_memory_counter++;
                }

                delete p_chunks;
                p_chunks = nullptr;
                this->chunk_map.erase(chunk_tag);
                continue;
            }

            current_loaded_chunk.push_back(p_chunks);
            this->chunk_map[chunk_tag] = p_chunks;
        }

        if (should_refresh_renderer) {
            agk::world::renderer()->refresh();
        }

        this->loaded_chunk_list = current_loaded_chunk;
        float scale_factor {static_cast<float>(chunk_size) / 128};
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
                p_chunk->transform.position.x = static_cast<float>(grid_pos.x * chunk_size);
                p_chunk->transform.position.z = static_cast<float>(grid_pos.y * chunk_size);
                p_chunk->id = (int32_t) ++this->wf_chunk_token_id;
                p_chunk->transform.scale = chunk_scale;

                /* Generate a metadata seed to this chunk. */
                p_chunk->metadata = {
                        agk::app.setting.chunk_fbm_frequency.get_value(),
                        agk::app.setting.chunk_fbm_amplitude.get_value(),
                        agk::app.setting.chunk_fbm_persistence.get_value(),
                        agk::app.setting.chunk_fbm_lacunarity.get_value(),
                        agk::app.setting.chunk_fbm_octaves.get_value()
                };

                this->queue_chunking.push(p_chunk);
                this->chunk_map[chunk_tag] = p_chunk;
            }
        }
    }

    if (!this->queue_chunking.empty() && util::reset_when(this->chunk_poll_chunking, 16)) {
        chunk *p_chunk {this->queue_chunking.front()};

        /* Use grid position as global UV for height map connection. */
        glm::vec2 global_uv {};
        glm::ivec2 grid_pos {};
        glm::ivec2 global_uv_grid {};

        int32_t chunk_size {agk::app.setting.chunk_dimension.get_value()};
        util::to_grid_pos(grid_pos, p_chunk->transform.position, {chunk_size, chunk_size});
        global_uv = grid_pos;

        /* Invoke parallel computation to randomize map. */
        this->parallel.p_program = (::asset::shader*) agk::asset::find("gpu/scripts.heightmap.generator");
        this->parallel.invoke();
        this->parallel.memory_barrier = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;

        this->parallel.p_program->set_uniform_float("uFrequency", p_chunk->metadata.frequency);
        this->parallel.p_program->set_uniform_float("uAmplitude", p_chunk->metadata.amplitude);
        this->parallel.p_program->set_uniform_float("uPersistence", p_chunk->metadata.persistence);
        this->parallel.p_program->set_uniform_float("uLacunarity", p_chunk->metadata.lacunarity);
        this->parallel.p_program->set_uniform_int("uOctaves", p_chunk->metadata.octaves);
        this->parallel.p_program->set_uniform_vec2("uGlobalUV", &global_uv[0]);

        /* Read nearest chunk metadata. */
        int32_t near_chunks {};
        int32_t tick {};

        bool should_check_into_shader {};
        float chunk_seedf[4] {};
        glm::vec3 near {};

        for (int32_t it {}; it < 4; it++) {
            global_uv_grid.x = (grid_pos.x + util::SURROUND[near_chunks++]);
            global_uv_grid.y = (grid_pos.y + util::SURROUND[near_chunks++]);

            std::string chunk_tag {std::to_string(global_uv_grid.x)};
            chunk_tag += 'x';
            chunk_tag += std::to_string(global_uv_grid.y);

            near.x = static_cast<float>(global_uv_grid.x);
            near.y = static_cast<float>(global_uv_grid.y);
            near.z = 1;

            /* Check if the metadata is different to infuse together. */
            auto &p_near_chunk {this->chunk_map[chunk_tag]};
            if (p_near_chunk != nullptr && p_chunk->is_processed()) {
                chunk_seedf[0] = p_near_chunk->metadata.frequency;
                chunk_seedf[1] = p_near_chunk->metadata.amplitude;
                chunk_seedf[2] = p_near_chunk->metadata.persistence;
                chunk_seedf[3] = p_near_chunk->metadata.lacunarity;

                chunk_tag = "uChunkMetadata[";
                chunk_tag += std::to_string(tick);
                chunk_tag += ']';

                this->parallel.p_program->set_uniform_vec4(chunk_tag + ".uSeedf", chunk_seedf);
                this->parallel.p_program->set_uniform_int(chunk_tag + ".uSeedi", p_near_chunk->metadata.octaves);
                if (!should_check_into_shader) should_check_into_shader = p_near_chunk->metadata != p_chunk->metadata;
            }

            chunk_tag = "uChunkMetadata[";
            chunk_tag += std::to_string(tick);
            chunk_tag += ']';
            this->parallel.p_program->set_uniform_vec3(chunk_tag + ".uGlobalUV", &near[0]);
            tick++;
        }

        this->parallel.p_program->set_uniform_bool("uCheckNearest", should_check_into_shader);

        this->parallel.dimension[0] = 128;
        this->parallel.dimension[1] = 128;
        this->parallel.dispatch_groups[0] = 1;
        this->parallel.dispatch_groups[1] = 1;

        uint32_t texture_key {static_cast<uint32_t>(p_chunk->id)};
        this->texture_chunk.invoke(texture_key, {GL_TEXTURE_2D, GL_FLOAT});
        this->texture_chunk.send<float>({128, 128, 1}, nullptr, {GL_RGBA32F, GL_RGBA});
        this->parallel.attach(0, this->texture_chunk[p_chunk->id], GL_WRITE_ONLY);
        this->parallel.attach(1, this->texture_vegetation[0], GL_READ_WRITE);
        this->parallel.dispatch();

        p_chunk->texture = this->texture_chunk[texture_key].id;
        this->loaded_chunk_list.push_back(p_chunk);
        agk::world::renderer()->add(p_chunk);

        /* Generate the trees objects. */
        this->texture_vegetation.invoke(0, {GL_TEXTURE_2D, GL_FLOAT});
        this->texture_vegetation.get<float>(this->vegetation_memory_list);
        this->texture_vegetation.revoke();

        this->queue_chunking.pop();
        this->parallel.revoke();

        int32_t index_length {static_cast<int32_t>(this->vegetation_memory_list[99 * 4])};

        glm::vec3 scale_factor {6.4F, 6.4F, 6.4F};
        glm::vec4 vegetation_pos {};
        glm::mat4 mat4x4_vegetation_model = glm::scale(glm::mat4(1.0f), p_chunk->transform.scale);
        glm::mat4 mat4x4_model {};

        for (int32_t it {}; it < index_length; it++) {
            float *p_vec {&(this->vegetation_memory_list[it * 4])};
            vegetation_pos.y = p_vec[2] * static_cast<float>(agk::app.setting.chunk_terrain_height.get_value());
            vegetation_pos = mat4x4_vegetation_model * vegetation_pos;

            vegetation_pos.x = p_vec[1] * (float) chunk_size;
            vegetation_pos.z = p_vec[3] * (float) chunk_size;

            object *p_tree {new object {}};
            p_tree->p_material = this->p_material_tree;
            p_tree->transform.position = vegetation_pos;
            p_tree->transform.scale = scale_factor;

            // Install an instance mesh to this vegetation.
            p_chunk->vegetation.push_back(p_tree->id);
            this->registry_wf(p_tree);
        }
    }

    if (util::reset_when(this->sky_cloud_timing, 256) && false) {
        glm::ivec3 dimension {8, 8, 8};
        this->parallel.p_program = (::asset::shader*) agk::asset::find("gpu/scripts.cloud.generator");
        this->parallel.invoke();

        this->parallel.dimension[0] = 8;
        this->parallel.dimension[1] = 8;
        this->parallel.dimension[2] = 8;
        this->parallel.dispatch_groups[0] = 1;
        this->parallel.dispatch_groups[1] = 1;
        this->parallel.dispatch_groups[2] = 1;
        this->parallel.memory_barrier = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;

        this->sky_cloud_texture.invoke(0, {GL_TEXTURE_3D, GL_FLOAT});
        this->sky_cloud_texture.send<float>(dimension, nullptr, {GL_RGBA, GL_RGBA16F});
        this->parallel.attach(0, this->sky_cloud_texture[0], GL_WRITE_ONLY);

        this->parallel.revoke();
        this->sky_cloud_texture.revoke();
    }

    if (this->free_memory_counter > 32) {
        std::vector<object*> new_wf_list {};
        this->wf_high_priority_list.clear();
        agk::app.p_renderer_service->wf_env_draw_list.clear();

        for (auto &p_world_feature : this->wf_list) {
            if (p_world_feature->is_dead || p_world_feature == nullptr) {
                delete p_world_feature;
                p_world_feature = nullptr;
                continue;
            }

            if (p_world_feature->get_priority() == enums::priority::high) {
                this->wf_high_priority_list.push_back(p_world_feature);
            }

            if (p_world_feature->get_visible() == enums::state::enable) {
                agk::app.p_renderer_service->wf_env_draw_list.push_back(p_world_feature);
            }

            new_wf_list.push_back(p_world_feature);
        }

        this->wf_list.clear();
        this->wf_list.insert(this->wf_list.end(), new_wf_list.begin(), new_wf_list.end());
        this->free_memory_counter = 0;
        util::log("Cleaned random trash memory");
    }
}

object *world::unregister_wf(object *p_world_feature) {
    if (p_world_feature != nullptr) {
        p_world_feature->is_dead = true;
        this->registered_wf_map.erase(p_world_feature->id);
        return p_world_feature;
    }

    // todo: unregister object from world.
    return nullptr;
}

object *&world::find_env_wf(int32_t wf_id) {
    return this->registered_wf_map[wf_id];
}

void world::on_event_changed_priority(SDL_Event &sdl_event) {
    auto *p_high {static_cast<bool*>(sdl_event.user.data1)};
    auto *p_wf_id {static_cast<int32_t*>(sdl_event.user.data2)};

    object *&p_world_feature_target {this->find_env_wf(*p_wf_id)};
    if (p_world_feature_target != nullptr && !*p_high && p_world_feature_target->get_priority() != enums::priority::low) {
        p_world_feature_target->set_priority(enums::priority::low, false);
        std::vector<object*> new_wf_high_priority_list {};
        for (object *&p_world_feature : this->wf_high_priority_list) {
            if (p_world_feature != nullptr && p_world_feature->get_priority() == enums::priority::high) {
                new_wf_high_priority_list.push_back(p_world_feature);
            }
        }

        this->wf_high_priority_list.clear();
        this->wf_high_priority_list.insert(this->wf_high_priority_list.end(), new_wf_high_priority_list.begin(), new_wf_high_priority_list.end());
    } else if (p_world_feature_target != nullptr && *p_high && p_world_feature_target->get_priority() != enums::priority::high) {
        p_world_feature_target->set_priority(enums::priority::high, false);
        this->wf_high_priority_list.push_back(p_world_feature_target);
    } else if (p_world_feature_target != nullptr && !*p_high) {
        p_world_feature_target->set_priority(enums::priority::low, false);
        this->wf_low_priority_queue.push(p_world_feature_target);
        this->poll_low_priority_queue = true;
    }

    delete p_wf_id;
    delete p_high;
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