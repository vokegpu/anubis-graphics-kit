#ifndef AGK_API_PROFILE_H
#define AGK_API_PROFILE_H

#include <SDL2/SDL.h>
#include "api/gc/gc.hpp"
#include "api/gpu/shading.hpp"
#include "api/world/world.hpp"
#include "api/world/camera/camera.hpp"
#include "api/mesh/mesh_loader.hpp"
#include "api/input/input.hpp"
#include "api/world/model/renderer.hpp"

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

    gc garbage_collector {};
    world world_client {};
    mesh_loader mesh3d_loader {};
    input input_manager {};
    renderer world_renderer {};

    std::map<std::string, shading::program*> shader_registry_map {};

};

#endif