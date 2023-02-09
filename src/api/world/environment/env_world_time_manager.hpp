#ifndef AGK_WORLD_ENVIRONMENT_TIME_H
#define AGK_WORLD_ENVIRONMENT_TIME_H

#include "api/feature/feature.hpp"
#include "api/util/env.hpp"
#include <glm/glm.hpp>

class world_time_manager : public feature {
public:
    util::timing delta_ms_real {};
    uint64_t delta_day_virtual {};
    uint64_t delta_si_real_elapsed {};
    uint64_t delta_hour_virtual {};
    uint64_t delta_min_virtual {};

    glm::vec3 color_nightmare {};
    glm::vec3 color_day {};
    glm::vec3 color_from_sky {};

    bool is_night {};
    float day_ambient_light {};
    float nightmare_ambient_light {};
    float ambient_light {};
    float ambient_next_light {};

    void on_create() override;
    void on_destroy() override;

    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif