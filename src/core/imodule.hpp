#ifndef AGK_API_FEATURE_H
#define AGK_API_FEATURE_H

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <unordered_map>

class imodule {
protected:
    static int32_t token;
    std::unordered_map<std::string, imodule*> component_map {};
public:
    bool is_enable {true};
    bool is_dead {};

    int32_t id {};
    std::string tag {};
    std::vector<imodule*> components {};

    imodule();
    ~imodule();

    virtual bool add(imodule *p_component);
    virtual bool remove(imodule *p_component);
    virtual imodule *get(std::string_view module_tag);

    virtual void on_create();
    virtual void on_destroy();
    virtual void on_event(SDL_Event &sdl_event);
    virtual void on_update();
    virtual void on_render();
};

#endif