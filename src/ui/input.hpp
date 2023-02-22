#ifndef AGK_UI_INPUT_H
#define AGK_UI_INPUT_H

#include <iostream>
#include <map>
#include "core/imodule.hpp"

class input : public imodule {
public:
    input() = default;
    ~input();

    std::map<std::string, bool> input_map {};
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
};

#endif