#ifndef AGK_IMPL_SCENE_STARTER_H
#define AGK_IMPL_SCENE_STARTER_H

#include "api/feature/feature.hpp"
#include "api/gpu/buffering.hpp"
#include "client/services/camera_manager.hpp"
#include "api/world/environment/light.hpp"
#include "api/world/environment/object.hpp"
#include <amogpu/amogpu.hpp>
#include <ekg/ekg.hpp>

namespace client::scenes {
    class starter : public feature {
    public:
        light *p_light_spot {};
        object *p_object_dino {};

        client::services::camera_manager *p_camera_manager {};
        font_renderer f_render {};
        dynamic_batching batching {};

        ekg::ui::slider *p_slider_light_intensity {};
        ekg::ui::slider *p_slider_base_speed {};
        ekg::ui::slider *p_slider_range {};
        ekg::ui::slider *p_slider_noise_control_x {};
        ekg::ui::slider *p_slider_noise_control_y {};
        ekg::ui::slider *p_slider_noise_control_offset {};
        ekg::ui::slider *p_slider_fog_dist {};
        ekg::ui::checkbox *p_checkbox_post_processing {};

        void on_create() override;
        void on_destroy() override;
        void on_event(SDL_Event &sdl_event) override;
        void on_update() override;
        void on_render() override;
    };
}

#endif