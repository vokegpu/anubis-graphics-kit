#include "env_entity.hpp"
#include "api/api.hpp"
#include "api/util/env.hpp"

entity::entity(model *p_model_linked) {
    this->p_model = p_model_linked;
    this->id = feature::token++;
    this->type = enums::type::entity;

    this->set_priority(enums::priority::high);
    this->set_visible(enums::state::enable);
}

entity::~entity() {

}

void entity::on_update() {
    feature::on_update();

    this->velocity += this->acceleration * api::dt;
    this->position += this->velocity * api::dt;
}
