#ifndef AGK_API_GPU_SHADING_ACCESS_H
#define AGK_API_GPU_SHADING_ACCESS_H

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
        GLuint shader_id {};
    };
}

struct shading_manager {
protected:
    std::map<std::string_view, GLuint> shading_programs_map {};
public:
    bool compile_shader(GLuint&, GLuint, const char*);
    bool create_shading_program(std::string_view, shading::program&, const std::vector<shading::resource>&);
    void quit();
};

#endif