#ifndef AGK_API_CORE_H
#define AGK_API_CORE_H

#include <SDL2/SDL.h>
#include "api/gc/gc.hpp"
#include "api/gpu/shading_access.hpp"
#include "api/world/world.hpp"
#include "api/world/camera/camera.hpp"
#include "api/mesh/mesh_loader.hpp"
#include "api/input/input.hpp"

class core {
public:
    SDL_Window* root {nullptr};
    int32_t screen_width {1600};
    int32_t screen_height {900};

    bool mainloop {};
    uint64_t fps {60};
    uint64_t display_fps {};

    feature* current_scene {nullptr};
    gc garbage_collector {};
    shading_manager shader_manger {};
    world world_client {};
    camera3d world_camera3d {};
    mesh_loader mesh3d_loader {};
    input input_manager {};
};

#endif