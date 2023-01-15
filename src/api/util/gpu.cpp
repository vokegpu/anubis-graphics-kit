#include "gpu.hpp"

GLuint util::gen_gl_texture(const glm::ivec2 &dimension, void *p_data, const glm::ivec3 &format, const glm::ivec2 &filter) {
    GLuint texture {};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter.x);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter.y);

    glTexImage2D(GL_TEXTURE_2D, 0, format.x, dimension.x, dimension.y, 0, format.y, format.z, p_data);
    if (filter == glm::ivec2 {GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR}) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}
