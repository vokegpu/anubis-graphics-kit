#include "math.hpp"

float util::clamp(float val, float min, float max) {
    return val > max ? max : (val < min ? min : val);
}

float util::min(float val, float min) {
    return val < min ? min : val;
}

float util::max(float val, float max) {
    return val > max ? max : val;
}

void util::mesh_list_vector3f(std::vector<float> &mesh_list, glm::vec3 vector3f) {
    mesh_list.push_back(vector3f.x);
    mesh_list.push_back(vector3f.y);
    mesh_list.push_back(vector3f.z);
}

void util::mesh_plane(mesh::data &data, glm::vec3 min, glm::vec3 max) {
    util::mesh_list_vector3f(data.vertices, {min.x, 0, min.z});
    util::mesh_list_vector3f(data.vertices, {min.x, 0, min.z + max.z});
    util::mesh_list_vector3f(data.vertices, {min.x, 0, min.z + max.z});
    util::mesh_list_vector3f(data.vertices, {min.x + max.x, 0, min.z + max.z});

    util::mesh_list_vector3f(data.vertices, {min.x + max.x, 0, min.z + max.z});
    util::mesh_list_vector3f(data.vertices, {min.x + max.x, 0, min.z});
    util::mesh_list_vector3f(data.vertices, {min.x + max.x, 0, min.z});
    util::mesh_list_vector3f(data.vertices, {min.x, 0, min.z});

    glm::vec3 n {0, 1, 0};
    util::mesh_list_vector3f(data.normals, n);
    util::mesh_list_vector3f(data.normals, n);
    util::mesh_list_vector3f(data.normals, n);
    util::mesh_list_vector3f(data.normals, n);

    util::mesh_list_vector3f(data.normals, n);
    util::mesh_list_vector3f(data.normals, n);
    util::mesh_list_vector3f(data.normals, n);
    util::mesh_list_vector3f(data.normals, n);

    data.contains_vertices = true;
    data.contains_normals = true;
}