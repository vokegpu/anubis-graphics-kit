#ifndef AGK_API_GPU_SHADING_H
#define AGK_API_GPU_SHADING_H

#include <iostream>
#include "GL/glew.h"
#include <map>
#include <vector>

namespace shading {
    enum stage {
        vertex = GL_VERTEX_SHADER,
        geometry = GL_GEOMETRY_SHADER,
        tessellation = GL_DONT_CARE,
        tessellation_evaluation = GL_DONT_CARE,
        fragment = GL_FRAGMENT_SHADER,
        computed = GL_DONT_CARE
    };

    struct program {
        GLuint id {};

        program();
        ~program();

        void set_uniform_vec4(std::string_view, const float*);
        void set_uniform_vec3(std::string_view, const float*);
        void set_uniform_vec2(std::string_view, const float*);
        void set_uniform_mat2(std::string_view, const float*);
        void set_uniform_mat3(std::string_view, const float*);
        void set_uniform_mat4(std::string_view, const float*);

        void set_uniform_float(std::string_view, float);
        void set_uniform_int(std::string_view, int32_t);
        void set_uniform_bool(std::string_view, bool);
    };

    struct resource {
        std::string_view path {};
        shading::stage stage {};
        bool is_source_code {};
    };
}

#endif