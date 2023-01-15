#ifndef AGK_API_H
#define AGK_API_H

#include "api/profile/profile.hpp"

namespace api {
    extern profile app;
    extern float dt;

    void mainloop(feature *p_scene_initial);
    void path(const char*);

    namespace service {
        void registry(feature *p_feature);
    };
    
    namespace scene {
        void load(feature *p_feature);
        feature *&current();
    }

    namespace gc {
        void destroy(feature *p_feature);
        void create(feature *p_feature);
    }

    namespace shading {
        bool create_program(std::string_view tag, ::shading::program *p_program, const std::vector<::shading::resource> &resource_list);
        bool find(std::string_view key, ::shading::program *&p_program);
        ::shading::program *registry(std::string_view key, ::shading::program *p_program);
    };

    namespace world {
        camera *&current_camera();
        entity *&current_player();

        ::world *&get();
        renderer *&renderer();

        model *create(std::string_view tag, std::string_view path, ::mesh::format format);
        void create(world_feature *p_world_feature);
        void destroy(world_feature *p_world_feature);
    };

    namespace mesh {
        mesh_loader &loader();
        bool load(::mesh::data&, std::string_view);
    };

    namespace input {
        bool pressed(std::string_view);
    };
}

#endif