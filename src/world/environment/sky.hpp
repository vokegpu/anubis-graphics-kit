#ifndef AGK_WORLD_ENVIRONMENT_SKY_H
#define AGK_WORLD_ENVIRONMENT_SKY_H

#include "core/imodule.hpp"
#include "util/env.hpp"
#include <glm/glm.hpp>
#include "gpu/gpubuffer.hpp"
#include "world/pbr/model.hpp"
#include "world/environment/light.hpp"
#include "gpu/gpuframebuffer.hpp"

class sky : public imodule {
protected:
    buffering buffer_tesseract {};
public:
    static bool isnight;
    static uint64_t hour;

    util::timing delta_ms_real {};
    uint64_t delta_day_virtual {};
    uint64_t delta_si_real_elapsed {};
    uint64_t delta_hour_virtual {};
    uint64_t delta_min_virtual {};

    glm::vec3 color_night {};
    glm::vec3 color_day {};
    glm::vec3 color_from_sky {};
    model *p_model_moon {};

    bool is_night {};
    light *p_astroo_light {};

    glm::vec3 moon_pos_time {};
    float day_ambient_light {};
    float night_ambient_light {};
    float ambient_light {};
    float ambient_next_light {};
    float ambient_luminance {};
    float stars_luminance {};
public:
    void set_time(int32_t hours, int32_t minutes);
    void on_create() override;
    void on_destroy() override;
    void on_event(SDL_Event &sdl_event) override;
    void on_update() override;
    void on_render() override;
};

#endif