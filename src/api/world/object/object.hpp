#ifndef AGK_API_WORLD_OBJECT_H
#define AGK_API_WORLD_OBJECT_H

#include "api/feature/feature.hpp"

class object : public feature {
public:
    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif
