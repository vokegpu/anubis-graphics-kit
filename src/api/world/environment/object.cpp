#include "object.hpp"

object::object(model *p_model_linked) {
    this->p_model = p_model_linked;
    this->id = feature::token++;
    this->type = enums::type::object;

    this->set_priority(enums::priority::high);
    this->set_visible(enums::state::enable);
}

object::~object() {
}
