#include "world.hpp"
#include "api/api.hpp"
#include "api/world/environment/entity.hpp"
#include "api/world/model/model.hpp"
#include "api/world/environment/object.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "api/event/event.hpp"
#include "api/util/env.hpp"

void world::registry_wf(world_feature *p_world_feature) {
	if (p_world_feature == nullptr) {
		return;
	}

    if (p_world_feature->id == 0) p_world_feature->id = feature::token++;
	api::gc::create(p_world_feature);

	this->registered_wf_map[p_world_feature->id] = p_world_feature;
	this->wf_list.push_back(p_world_feature);
}

void world::on_create() {
    shading::program *p_program_material_pbr {new ::shading::program {}};
    api::shading::createprogram("MaterialPBR", p_program_material_pbr,  {
        {"./data/effects/MaterialPBR.vsh", shading::stage::vertex},
        {"./data/effects/MaterialPBR.fsh", shading::stage::fragment}
    });
}

void world::on_destroy() {

}

void world::on_event(SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_WINDOWEVENT: {
            api::app.p_current_camera->on_event(sdl_event);
            break;
        }

        case SDL_USEREVENT: {
            switch (sdl_event.user.code) {
                case event::WORLD_REFRESH_DRAW: {
                    this->on_event_refresh_draw(sdl_event);
                    break;
                }

                case event::WORLD_REFRESH_PRIORITY: {
                    this->on_event_refresh_priority(sdl_event);
                    break;
                }
            }

            break;
        }
    }
}

void world::on_update() {
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
}

void world::on_render() {
    /* Terrain rendering section. */
    /* Here... */

    /* Object & entity rendering section. */

    auto camera {api::world::currentcamera()};
    glm::mat4 mvp {camera->get_perspective() * camera->get_view()};
    glm::mat4 mat4x4_model {};
    glm::mat3 cubic_normal_matrix {};

    shading::program *p_program_material_pbr {};
    api::shading::find("MaterialPBR", p_program_material_pbr);

    entity *p_entity {};
    model *p_model {};
    object *p_object {};

    enums::material material_type {};

    glUseProgram(p_program_material_pbr->id);
    int32_t current_light_loaded {};
    std::string light_index_tag {};

	for (world_feature *&p_world_feature : this->wf_draw_list) {
        if (p_world_feature == nullptr) {
            continue;
        }

        switch (p_world_feature->type) {
            case enums::type::entity: {
                p_entity = (entity*) p_world_feature;
                p_model = p_entity->p_model;
                break;
            }

            case enums::type::object: {
                p_object = (object*) p_world_feature;
                p_model = p_object->p_model;
                break;
            }

            default: {
                break;
            }
        }

        if (p_model == nullptr || p_object->p_material == nullptr) {
            continue;
        }

        mat4x4_model = glm::mat4(1.0f);
        mat4x4_model = glm::translate(mat4x4_model, p_object->position);
        mat4x4_model = glm::scale(mat4x4_model, p_object->scale);

        cubic_normal_matrix = glm::inverse(glm::transpose(glm::mat3(mat4x4_model)));
        p_program_material_pbr->set_uniform_mat3("MatrixNormal", &cubic_normal_matrix[0][0]);
        p_program_material_pbr->set_uniform_mat4("MatrixModel", &mat4x4_model[0][0]);

        mat4x4_model = mvp * mat4x4_model;
        p_program_material_pbr->set_uniform_mat4("MVP", &mat4x4_model[0][0]);

        material_type = p_object->p_material->get_type();
        p_program_material_pbr->set_uniform_bool("Material.Metal", material_type == enums::material::metal);
        p_program_material_pbr->set_uniform_float("Material.Rough", p_object->p_material->get_rough());

        p_model->buffering.draw();
    }

    if (this->loaded_light_size != current_light_loaded) {
        this->loaded_light_size = current_light_loaded;
        p_program_material_pbr->set_uniform_int("LoadedLightLen", current_light_loaded);
    };

    glUseProgram(0);
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

void world::on_event_refresh_draw(SDL_Event &sdl_event) {
    auto *p_to_remove {static_cast<bool*>(sdl_event.user.data1)};
    auto *p_wf_id {static_cast<int32_t*>(sdl_event.user.data2)};

    world_feature *p_world_feature {this->find(*p_wf_id)};
    if (*p_to_remove) {
        this->wf_draw_list.clear();
        if (p_world_feature != nullptr) p_world_feature->set_visible(enums::state::disable, false);

        for (world_feature *&p_world_features : this->wf_list) {
            if (p_world_features != nullptr && p_world_features->get_visible() == enums::state::enable) {
                this->wf_draw_list.push_back(p_world_features);
            }
        }
    } else if (p_world_feature != nullptr && p_world_feature->get_visible() != enums::state::enable) {
        this->wf_draw_list.push_back(p_world_feature);
        p_world_feature->set_visible(enums::state::enable, false);
    }

    delete p_to_remove;
    delete p_wf_id;
}

void world::on_event_refresh_priority(SDL_Event &sdl_event) {
    auto *p_high {static_cast<bool*>(sdl_event.user.data1)};
    auto *p_wf_id {static_cast<int32_t*>(sdl_event.user.data2)};

    world_feature *p_world_feature_target {this->find(*p_wf_id)};
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

world_feature *world::find(int32_t wf_id) {
    return this->registered_wf_map[wf_id];
}
