#ifndef AGK_API_H
#define AGK_API_H

#include "api/core/core.hpp"

namespace api {
	extern core app;
	void mainloop(feature*);
    void path(const char*);

	namespace scene {
		void load(feature*);
		feature* &current();
	}

	namespace gc {
		void destroy(feature*);
		void create(feature*);
	}

    namespace shading {
        bool createprogram(std::string_view, ::shading::program &program, const std::vector<::shading::resource>&);
    };

    namespace world {
        camera3d &camera3d();
        ::world &current();
        world_render &render();
        void create(object*);
        void destroy(object*);
    };

    namespace mesh {
        bool load(::mesh::data&, std::string_view);
        void compile(::mesh::data &data, buffer_builder *model);
    };

    namespace input {
        bool pressed(std::string_view);
    };
}

#endif