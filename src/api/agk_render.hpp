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
    std::vector<float> data_normal;

    uint32_t vertex_amount = 0;
public:
    void clear();
    void dispatch(float x, float y, float z, float u, float v);
    void dispatch(uint32_t sizeof_list, float* list);
    void dispatch_normal(float x, float y, float z);

    std::vector<float> &access_data();
    std::vector<float> &access_data_normals();

    uint32_t get_vertex_amount();
    uint32_t size();

    void operator += (agk_mesh_stream &mesh) {
        this->data.insert(this->data.begin(), mesh.access_data().begin(), mesh.access_data().end());
        this->data_normal.insert(this->data_normal.begin(), mesh.access_data_normals().begin(), mesh.access_data().end());
        this->vertex_amount += mesh.get_vertex_amount();
    }
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

namespace model {
    enum type {
        OBJ
    };

    bool load(agk_mesh_stream &mesh, const std::string &path, model::type type);
};

#endif