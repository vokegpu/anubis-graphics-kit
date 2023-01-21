#ifndef AGK_API_PROFILE_H
#define AGK_API_PROFILE_H

#include <SDL2/SDL.h>
#include "api/gc/gc.hpp"
#include "api/gpu/program.hpp"
#include "api/world/world.hpp"
#include "api/world/camera/camera.hpp"
#include "api/mesh/mesh_loader.hpp"
#include "api/input/input.hpp"
#include "api/world/renderer/renderer.hpp"
#include "api/world/environment/entity.hpp"

class profile {
public:
    SDL_Window *p_sdl_window {nullptr};
    int32_t screen_width {1600};
    int32_t screen_height {900};

    bool mainloop {};
    uint64_t fps {60};
    uint64_t display_fps {};

    feature *p_current_scene {nullptr};
    camera *p_current_camera {nullptr};
    entity *p_current_player {nullptr};
    world *p_world_client {nullptr};
    renderer *p_world_renderer {nullptr};

    gc garbage_collector {};
    mesh_loader mesher_loader {};
    input input_manager {};
    value_global values {};

    std::map<std::string, shading::program*> shader_registry_map {};
    std::vector<feature*> loaded_service_list {};
};

#endif