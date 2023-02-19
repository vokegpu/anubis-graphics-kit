#ifndef AGK_API_H
#define AGK_API_H

#include "api/profile/profile.hpp"

namespace api {
    extern profile app;
    extern float dt;

    void mainloop(feature *p_scene_initial);
    void path(const char*);
    void viewport();

    namespace task {
        void registry(feature *p_feature);
        void synchronize(feature *p_feature);
        void populate();
    };
    
    namespace scene {
        void load(feature *p_feature);
        feature *&current();
    }

    namespace world {
        camera *&current_camera();
        entity *&current_player();

        ::world *&get();
        renderer *&renderer();
        world_time_manager *&time_manager();

        model *create(std::string_view tag, std::string_view path, ::mesh::format format);
        model *create(std::string_view tag, std::string_view path, ::mesh::data &mesh);

        void create(world_feature *p_world_feature);
        void destroy(world_feature *p_world_feature);
    };

    namespace asset {
        feature *find(std::string_view asset_name);
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