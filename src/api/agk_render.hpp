#pragma once

#ifndef AGK_RENDER_H
#define AGK_RENDER_H

#include "agk_gpu.hpp"
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

struct agk_mesh_stream {
protected:
    std::vector<float> data;
    uint32_t vertex_amount = 0;
public:
    void clear();
    void dispatch(float x, float y, float z, float u, float v);
    void dispatch(uint32_t sizeof_list, float* list);

    std::vector<float> &access_data();
    uint32_t get_vertex_amount();
    uint32_t size();
};

class agk_batch3d {
protected:
    GLuint vbo_data1 = 0;
    GLuint vbo_data2 = 0;
    GLuint vao_all_data = 0;

    agk_mesh_stream mesh {};
    bool should_not_create_buffers = false;
public:
    static gpu::program fx_shape3d;
    static void init();

    void invoke();
    void dispatch(agk_mesh_stream &m_stream);
    void revoke();
    void draw(const glm::vec3 &pos, const glm::vec4 &color);

};

#endif