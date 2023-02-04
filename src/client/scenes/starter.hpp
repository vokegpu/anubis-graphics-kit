#ifndef AGK_IMPL_SCENE_STARTER_H
#define AGK_IMPL_SCENE_STARTER_H

#include "api/feature/feature.hpp"
#include "api/gpu/tools.hpp"
#include "client/services/camera_manager.hpp"
#include "api/world/environment/env_light.hpp"
#include "api/world/environment/env_object.hpp"
#include <ekg/ekg.hpp>

namespace client::scenes {
    class starter : public feature {
    public:
        light *p_light_spot {};
        object *p_object_dino {};

        client::services::camera_manager *p_camera_manager {};

        ekg::ui::slider *p_slider_light_intensity {};
        ekg::ui::slider *p_slider_range {};
        ekg::ui::slider *p_slider_fog_dist {};
        ekg::ui::checkbox *p_checkbox_post_processing {};
        ekg::ui::checkbox *p_check_box_hdr {};
        ekg::ui::slider *p_slider_hdr_exposure {};

        ekg::ui::slider *p_frequency;
        ekg::ui::slider *p_amplitude;
        ekg::ui::slider *p_persistence;
        ekg::ui::slider *p_lacunarity;
        ekg::ui::slider *p_octaves;

        void on_create() override;
        void on_destroy() override;
        void on_event(SDL_Event &sdl_event) override;
        void on_update() override;
        void on_render() override;
    };
}

#endif