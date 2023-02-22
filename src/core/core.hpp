#ifndef AGK_CORE_H
#define AGK_CORE_H

#include <SDL2/SDL.h>
#include "world/world.hpp"
#include "world/camera/camera.hpp"
#include "asset/mesh_loader.hpp"
#include "ui/input.hpp"
#include "world/renderer/renderer.hpp"
#include "world/environment/entity.hpp"
#include "world/environment/sky.hpp"
#include "asset/asset_manager.hpp"

class core {
public:
    SDL_Window *p_sdl_window {nullptr};
    int32_t screen_width {1600};
    int32_t screen_height {900};

    bool mainloop {};
    uint64_t fps {60};
    uint64_t display_fps {};
    float background[3] {1.0f, 1.0f, 1.0f};

    imodule *p_curr_scene {};
    camera *p_curr_camera {};
    entity *p_curr_player {};

    world *p_world_service {};
    renderer *p_renderer_service {};
    asset_manager *p_asset_manager_service {};
    sky *p_sky {};
    input *p_input_service {};

    mesh_loader mesh_loader_manager {};
    value_global setting {};

    std::vector<imodule*> loaded_service_list {};
    std::queue<imodule*> task_queue {};
};

#endif