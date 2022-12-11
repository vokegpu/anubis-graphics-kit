#include "terrain.hpp"
#include "api/util/math.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "api/api.hpp"

void terrain::on_create() {
    this->buffring.mode = buffer_builder_mode::instanced;
    this->buffring.primitive = GL_TRIANGLES;
    this->buffring.invoke();
    
    mesh::data mesh_data {};
    util::mesh_plane(mesh_data, {0, 0, 0}, {1, 1, 1});
    int32_t range {255}, plane {range / 2};
    glm::mat4 model {}, transpose_model {};

    for (int32_t x {-plane}; x < plane; x++) {
        for (int32_t y {-plane}; y < plane; y++) {
            model = glm::mat4(1);
            model = glm::translate(model, {x, 0, y});
            transpose_model = glm::inverseTranspose(glm::mat3(transpose_model));

            auto ptr {glm::value_ptr(model)};
            mesh_data.instanced_vertices.insert(mesh_data.instanced_vertices.end(), &ptr[0], &ptr[16]);

            ptr = glm::value_ptr(model);
            mesh_data.instanced_normals.insert(mesh_data.instanced_normals.end(), &ptr[0], &ptr[16]);
        }
    }
    
    this->buffring.instanced_size = range;
    
    this->buffring.bind();
    this->buffring.send_data(mesh_data.vertices.size() * sizeof(GLfloat), &mesh_data.vertices[0], GL_STATIC_DRAW);

    this->buffring.bind();
    this->buffring.send_data(range * sizeof(glm::mat4), &mesh_data.instanced_vertices[0], GL_STATIC_DRAW);
    this->buffring.shader_instanced(2, 4, 4, sizeof(glm::mat4));

    this->buffring.bind();
    this->buffring.send_data(range * sizeof(glm::mat4), &mesh_data.instanced_normals[0], GL_STATIC_DRAW);
    this->buffring.shader_instanced(3, 4, 4, sizeof(glm::mat4));
    this->buffring.revoke();
}

void terrain::on_destroy() {

}

void terrain::on_event(SDL_Event &sdl_event) {

}

void terrain::on_update() {

}

void terrain::on_render() {
    auto &world_render {api::world::render()};
    glUseProgram(world_render.terrain_model_shading.id);

    this->buffring.on_render();
    glUseProgram(0);
}