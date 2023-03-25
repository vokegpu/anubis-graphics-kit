#ifndef AGK_GPU_TOOLS_H
#define AGK_GPU_TOOLS_H

#include "GL/glew.h"
#include <iostream>

namespace gpu {
    typedef struct texture {
        unsigned int w {};
        unsigned int h {};
        unsigned int d {};
        unsigned int type {};
        unsigned int format {};
        unsigned int channel {};
        unsigned int id {};
        unsigned int primitive {};
    } texture;

    typedef struct frame {
        int w {};
        int h {};
        int z {};
        unsigned int type {};
        unsigned int format {};
        unsigned int id_texture {};
        unsigned int id_fbo {};
        unsigned int id_renderbuffer {};
        unsigned int id_depth {};
    } frame;

    int64_t channels(gpu::texture &texture);

    template<typename t>
    t *read(gpu::texture &texture, t *p_data) {
        /* Get content from texture. */
        glGetTexImage(texture.type, 0, texture.channel, texture.primitive, p_data);
        return p_data;
    }
}

#endif