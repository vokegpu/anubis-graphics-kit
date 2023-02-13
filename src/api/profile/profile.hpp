#ifndef AGK_API_PROFILE_H
#define AGK_API_PROFILE_H

#include <SDL2/SDL.h>
#include "api/gpu/program.hpp"
#include "api/world/world.hpp"
#include "api/world/camera/camera.hpp"
#include "api/mesh/mesh_loader.hpp"
#include "api/input/input.hpp"
#include "api/world/renderer/renderer.hpp"
#include "api/world/environment/env_entity.hpp"
#include "api/world/renderer/dynamic_renderer.hpp"
#include "api/world/environment/env_world_time_manager.hpp"

class profile {
public:
    SDL_Window *p_sdl_window {nullptr};
    int32_t screen_width {1600};
    int32_t screen_height {900};

    bool mainloop {};
    uint64_t fps {60};
    uint64_t display_fps {};
    float background[3] {1.0f, 1.0f, 1.0f};

    feature *p_current_scene {};
    camera *p_current_camera {};
    entity *p_current_player {};
    world *p_world_client {};
    renderer *p_world_renderer {};
    world_time_manager *p_world_time_manager {};
    dynamic_geometry_instanced_renderer *p_world_dynamic_geometry_renderer {};

    mesh_loader mesher_loader {};
    input input_manager {};
    value_global setting {};

    std::map<std::string, shading::program*> shader_registry_map {};
    std::vector<feature*> loaded_service_list {};
    std::queue<feature*> task_queue {};
};

#endif