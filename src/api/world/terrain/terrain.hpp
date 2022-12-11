#ifndef AGK_API_WORLD_TERRAIN_H
#define AGK_API_WORLD_TERRAIN_H

#include "api/feature/feature.hpp"
#include "api/gpu/buffer_builder.hpp"

class terrain : public feature {
protected:
    buffer_builder buffring {"terrain-buffering"};
public:
    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif