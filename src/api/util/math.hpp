#ifndef AGK_API_UTIL_MATH_H
#define AGK_API_UTIL_MATH_H

#include "api/mesh/mesh.hpp"
#include <glm/glm.hpp>

namespace util {
    float clamp(float val, float min, float max);
    float min(float val, float min);
    float max(float val, float max);

    void mesh_list_vector3f(std::vector<float>&, glm::vec3);
    void mesh_plane(mesh::data&, glm::vec3, glm::vec3);
}

#endif