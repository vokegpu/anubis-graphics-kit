#include "agk_feature.hpp"

agk_feature::agk_feature() {

}

agk_feature::~agk_feature() {

}

void agk_feature::set_tag(const std::string &str) {
    this->tag = str;
}

std::string agk_feature::get_tag() {
    return this->tag;
}

void agk_feature::on_event(SDL_Event &sdl_event) {
    
}

void agk_feature::on_update() {

}

void agk_feature::on_render() {

}
