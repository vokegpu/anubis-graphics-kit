#include "gpu.hpp"

int64_t gpu::channels(gpu::texture &texture) {
    switch (texture.channel) {
        case GL_RG: {
            return 2;
        }

        case GL_RGB: {
            return 3;
        }

        case GL_RGBA: {
            return 4;
        }

        case GL_RG_INTEGER: {
            return 2;
        }

        case GL_RGB_INTEGER: {
            return 3;
        }

        case GL_RGBA_INTEGER: {
            return 4;
        }
    }

    return 1;
}