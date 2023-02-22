#include "imodule.hpp"
#include "util/env.hpp"
#include <iostream>
#include <algorithm>

int32_t imodule::token {1};

void imodule::on_create() {

}

void imodule::on_destroy() {

}

void imodule::on_event(SDL_Event &sdl_event) {

}

void imodule::on_update() {

}

void imodule::on_render() {

}

imodule::imodule() {

}

imodule::~imodule() {
}

bool imodule::add(imodule *p_component) {
    if (p_component == nullptr) {
        return false;
    }

    this->components.push_back(p_component);
    return true;
}

bool imodule::remove(imodule *p_component) {
    if (p_component == nullptr) {
        return false;
    }

    static imodule *pp = p_component;
    std::vector<imodule*>::iterator it {std::remove_if(this->components.begin(), this->components.end(), [](imodule *p) {
        return p == pp;
    })};

    return true;
}

imodule *imodule::get(std::string_view module_tag) {
    return this->component_map[module_tag.data()];
}
