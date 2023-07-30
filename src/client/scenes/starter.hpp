#ifndef AGK_CLIENT_SERVICES_STARTER_H
#define AGK_CLIENT_SERVICES_STARTER_H

#include "core/imodule.hpp"
#include "gpu/gpubuffer.hpp"
#include "world/environment/light.hpp"
#include "world/environment/object.hpp"
#include <ekg/ekg.hpp>

namespace client::scenes {
    class starter : public imodule {
    protected:
        buffering buffer_test {};
        void do_test_overlay();
        void do_json_test();
    public:
        light *p_light_spot {};
        object *p_object_dino {};
        uint64_t last_display_fps {};
    public:
        ekg::ui::slider *p_time_now {};
        ekg::ui::slider *p_light_intensity {};
        ekg::ui::slider *p_chunk_range {};
        ekg::ui::slider *p_fog_distance {};

        ekg::ui::slider *p_frequency {};
        ekg::ui::slider *p_amplitude {};
        ekg::ui::slider *p_persistence {};
        ekg::ui::slider *p_lacunarity {};
        ekg::ui::slider *p_octaves {};

        ekg::ui::checkbox *p_enable_post_processing {};
        ekg::ui::checkbox *p_enable_hdr {};
        ekg::ui::slider *p_hdr_exposure {};
        ekg::ui::checkbox *p_enable_motion_blur {};
        ekg::ui::slider *p_motion_blur_intensity {};

        ekg::ui::frame *p_top_left_frame {};
        ekg::ui::frame *p_bottom_frame {};
    public:
        void init_gui();
        void refresh_gui();
    public:
        void on_create() override;
        void on_destroy() override;
        void on_event(SDL_Event &sdl_event) override;
        void on_update() override;
        void on_render() override;
    };
}

#endif