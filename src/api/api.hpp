#ifndef AGK_API_H
#define AGK_API_H

#include "api/core/core.hpp"

namespace api {
    extern core app;
    extern float dt;

    void mainloop(feature *p_feature);
    void path(const char*);
    
    namespace scene {
        void load(feature *p_feature);
        feature* &current();
    }

    namespace gc {
        void destroy(feature *p_feature);
        void create(feature *p_feature);
    }

    namespace shading {
        bool createprogram(std::string_view, ::shading::program *p_program, const std::vector<::shading::resource> &resource_list);
        bool find(std::string_view key, ::shading::program *p_program);
        ::shading::program *registry(std::string_view key, ::shading::program *p_program);
    };

    namespace world {
        camera *currentcamera();
        ::world &current();
        void create(world_feature *p_world_feature);
        void destroy(world_feature *p_world_feature);
    };

    namespace mesh {
        bool load(::mesh::data&, std::string_view);
    };

    namespace input {
        bool pressed(std::string_view);
    };
}

#endif