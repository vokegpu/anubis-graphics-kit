#include "entity.hpp"
#include "agk.hpp"

entity::entity(::asset::model *p_model_linked) {
    this->p_model = p_model_linked;
    this->id = imodule::token++;
    this->type = enums::type::entity;

    this->set_priority(enums::priority::high);
    this->set_visible(enums::state::enable);
}

entity::~entity() {

}

void entity::on_update() {
    this->velocity += this->acceleration * agk::dt;
    this->transform.position += this->velocity * agk::dt;
}
