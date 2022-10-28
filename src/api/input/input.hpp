#ifndef AGK_API_INPUT_H
#define AGK_API_INPUT_H

#include <iostream>
#include <map>
#include "api/feature/feature.hpp"

class input : public feature {
public:
    std::map<std::string, bool> input_map {};
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
};

#endif