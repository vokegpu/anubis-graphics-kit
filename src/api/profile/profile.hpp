#ifndef AGK_API_PROFILE_H
#define AGK_API_PROFILE_H

#include <SDL2/SDL.h>
#include "api/world/world.hpp"
#include "api/world/camera/camera.hpp"
#include "api/mesh/mesh_loader.hpp"
#include "api/input/input.hpp"
#include "api/world/renderer/renderer.hpp"
#include "api/world/environment/entity.hpp"
#include "api/world/environment/world_time_manager.hpp"
#include "api/asset/manager/asset_manager.hpp"

class profile {
public:
    SDL_Window *p_sdl_window {nullptr};
    int32_t screen_width {1600};
    int32_t screen_height {900};

    bool mainloop {};
    uint64_t fps {60};
    uint64_t display_fps {};
    float background[3] {1.0f, 1.0f, 1.0f};

    feature *p_curr_scene {};
    camera *p_curr_camera {};
    entity *p_curr_player {};

    world *p_world_service {};
    renderer *p_renderer_service {};
    asset_manager *p_asset_manager_service {};
    world_time_manager *p_world_time_service {};
    input *p_input_service {};

    mesh_loader mesh_loader_manager {};
    value_global setting {};

    std::vector<feature*> loaded_service_list {};
    std::queue<feature*> task_queue {};
};

#endif