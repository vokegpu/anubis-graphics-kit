#include "entity.hpp"
#include "api/api.hpp"

void entity::on_update() {
    feature::on_update();

    this->velocity += this->acceleration * api::dt;
    this->position += this->velocity * api::dt;
}
