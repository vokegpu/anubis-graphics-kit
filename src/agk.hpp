#ifndef AGK_H
#define AGK_H

#include "core/core.hpp"

namespace agk {
    extern core app;
    extern float dt;

    void mainloop(imodule *p_scene_initial);
    void path(const char*);
    void viewport();
    void setfps(uint32_t fps, bool vsync = agk::app.vsync);

    namespace task {
        void registry(imodule *p_feature);
        void synchronize(imodule *p_feature);
        void populate();
    };
    
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
    };

    namespace asset {
        void load(imodule *p_asset);
        imodule *find(std::string_view asset_name);
    }

    namespace mesh {
        mesh_loader &loader();
        bool load(::mesh::data&, std::string_view);
    };

    namespace input {
        bool pressed(std::string_view);
    };
}

#endif