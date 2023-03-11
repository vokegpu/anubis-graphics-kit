#ifndef AGK_CORE_H
#define AGK_CORE_H

#include <SDL2/SDL.h>
#include "world/world.hpp"
#include "world/camera/camera.hpp"
#include "mesh/meshloader.hpp"
#include "ui/input.hpp"
#include "world/renderer/renderer.hpp"
#include "world/environment/entity.hpp"
#include "world/environment/sky.hpp"
#include "asset/assetmanager.hpp"
#include "world/pbr/pbrloader.hpp"

class core {
public:
    static std::vector<imodule*> renderablelist;
    static std::vector<imodule*> updateablelist;
    static std::vector<imodule*> listenablelist;
    static std::queue<imodule*> taskqueue;
    static int32_t token {};
public:
    SDL_Window *p_sdl_window {nullptr};
    int32_t screen_width {1600};
    int32_t screen_height {900};

    bool mainloop {};
    uint64_t fps {60};
    uint64_t display_fps {};
    bool vsync {};
    float background[3] {1.0f, 1.0f, 1.0f};

    imodule *p_curr_scene {};
    camera *p_curr_camera {};
    entity *p_curr_player {};

    world *p_world_service {};
    renderer *p_renderer_service {};
    assetmanager *p_asset_manager_service {};
    sky *p_sky {};
    input *p_input_service {};
    usercamera *p_user_camera {};

    meshloader mesh_loader_manager {};
    valueglobal setting {};
};

#endif