#ifndef AGK_API_WORLD_H
#define AGK_API_WORLD_H

#include <vector>
#include <map>
#include "api/feature/feature.hpp"
#include "api/world/entity/entity.hpp"
#include "api/world/object/object.hpp"

class world : public feature {
public:
    std::map<uint32_t, object*> loaded_object_map {};
    std::vector<object*> loaded_object_list {};
    std::vector<entity*> loaded_entity_list {};

    bool camera_movement {};
    entity *player {};

    void do_camera_movement();
    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif