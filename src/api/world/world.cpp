#include "world.hpp"
#include "api/api.hpp"
#include "api/gc/memory.hpp"
#include "api/world/environment/entity.hpp"
#include "api/world/model/model.hpp"
#include "api/world/environment/object.hpp"
#include <glm/gtc/matrix_transform.hpp>

void world::registry_wf(world_feature *p_world_feature) {
	if (p_world_feature == nullptr) {
		return;
	}

    p_world_feature->id = ++this->wf_token_id;
	api::gc::create(p_world_feature);

	this->registered_wf_map[p_world_feature->id] = p_world_feature;
	this->wf_list.push_back(p_world_feature);
}

void world::refresh_wf_renderer() {
	this->wf_draw_list.clear();

	for (world_feature *&p_world_feature : this->wf_list) {
		if (p_world_feature != nullptr && p_world_feature->visible == enums::state::enable) {
			this->wf_draw_list.push_back(p_world_feature);
		}
	}
}

void world::append_process(world_feature *p_world_feature) {
	if (memory::check(p_world_feature->id) == nullptr) {
		memory::emmite(p_world_feature->id, p_world_feature);
		this->wf_process_queue.push(p_world_feature);
	}
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
    auto camera {api::world::currentcamera()};
    if (sdl_event.type == SDL_WINDOWEVENT) {
        camera->on_event(sdl_event);
    }
}

void world::on_update() {
	while (!this->wf_process_queue.empty()) {
		auto &p_world_feature {this->wf_process_queue.front()};
		if (p_world_feature != nullptr) p_world_feature->on_update();
		this->wf_process_queue.pop();
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

    pbrm_light *p_pbrm_light {};
    pbrm_solid *p_pbrm_solid {};
    enums::material material_type {};

    glUseProgram(p_program_material_pbr->id);

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

        if (p_model == nullptr) {
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

        material_type = p_world_feature->p_pbrm->get_type();
        switch (material_type) {
            case enums::material::light: {
                p_pbrm_light = (pbrm_light*) p_world_feature->p_pbrm;

                p_program_material_pbr->set_uniform_bool("Material.Color", &p_pbrm_light->get_intensity()[0]);
                p_program_material_pbr->set_uniform_bool("MaterialLightSpot", true);
                break;
            }

            default: {
                p_pbrm_solid = (pbrm_solid*) p_world_feature->p_pbrm;

                p_program_material_pbr->set_uniform_bool("MaterialLightSpot", false);
                p_program_material_pbr->set_uniform_bool("Material.Solid.Metal", material_type == enums::material::metal);
                p_program_material_pbr->set_uniform_float("Material.Solid.Rough",p_pbrm_solid->get_rough());

                break;
            }
        }

        p_model->buffering.draw();
    }

    glUseProgram(0);
}

world_feature *world::unregister_wf(world_feature *p_world_feature) {
    if (p_world_feature != nullptr) {
        this->registered_wf_map.erase(p_world_feature->id);
        api::gc::destroy(p_world_feature);
        return p_world_feature;
    }

    return nullptr;
}
