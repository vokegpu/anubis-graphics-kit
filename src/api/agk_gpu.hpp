#pragma once

#ifndef AGK_GPU_H
#define AGK_GPU_H

#include <iostream>
#include <string>
#include <gl/glew.h>

/**
 * Access GPU functions.
 **/
namespace gpu {
    struct program {
        bool validation;
        GLuint program;

        void set1f(const std::string &uniform_name, float val);
        void set1i(const std::string &uniform_name, int32_t val);
        void set1ui(const std::string &uniform_name, uint32_t val);
        void set4f(const std::string &uniform_name, const float* val);
        void setm4f(const std::string &uniform_name, const float* val);
        void use();
    };

    bool compile_shader(GLuint &shader, GLuint mode, const char* shader_src);
    void create_program(gpu::program &program, const char* vsh_path, const char* fsh_path, const char* gsh_path = nullptr);
}

#endif