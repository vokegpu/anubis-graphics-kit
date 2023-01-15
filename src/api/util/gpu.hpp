#ifndef AGK_API_UTIL_GPU_H
#define AGK_API_UTIL_GPU_H

#include <glm/glm.hpp>
#include "GL/glew.h"

namespace util {
    GLuint gen_gl_texture(const glm::ivec2 &dimension,
                            void *p_data,
                            const glm::ivec3 &format,
                            const glm::ivec2 &filter = {GL_LINEAR, GL_LINEAR});
}

#endif