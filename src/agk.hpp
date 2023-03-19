#ifndef AGK_H
#define AGK_H

#include "core/core.hpp"

/*
 * Main Anubis Graphics Kit namespace contains incorrect case style -
 * "soon it should be fixed".
 */
namespace agk {
    extern core app;
    extern float dt;

    void mainloop(imodule *p_scene_initial);
    void path(const char*);
    void viewport();
    void setfps(uint32_t fps, bool vsync = agk::app.vsync);

    enum service {
        updateable = 1,
        renderable = 2,
        listenable = 4;
    }

    namespace task {
        void registry(imodule *p_feature, uint16_t flags);
        void synchronize(imodule *p_feature);
        void populate();
    }
    
    namespace flags {
        bool contains(uint16_t &target, uint16_t check);
    }

    namespace scene {
        void load(imodule *p_scene);
        imodule *&current();
    }

    namespace world {
        camera *&current_camera();
        entity *&current_player();

        ::world *&get();
        renderer *&renderer();
        sky *&sky();

        void create(object *p_object);
        void destroy(object *p_object);
    }

    namespace asset {
        void load(imodule *p_asset);
        imodule *find(std::string_view asset_name);
    }

    namespace stream {
        streamparser &parser();

        bool load(::stream::mtl &mtl, std::string_view path);
        bool load(::stream::mesh &mesh, std::string_view path);
    }

    namespace pbr {
        bool loadmaterial(std::vector<std::string> &loaded_material_list, std::string_view path);
        bool loadmodel(std::string_view tag, std::vector<std::string> &loaded_model_list, std::string_view path);
        material *findmaterial(std::string_view k_name);
        model *findmodel(std::string_view k_name);
    }

    namespace ui {
        usercamera *&get_user_camera();
        bool input(std::string_view);
    }
}

#endif