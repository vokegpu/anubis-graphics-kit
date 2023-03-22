#ifndef AGK_WORLD_PBR_MODEL_H
#define AGK_WORLD_PBR_MODEL_H

#include "gpu/gpu_buffer.hpp"
#include "core/imodule.hpp"
#include <functional>
#include "util/math.hpp"

class model : public imodule {
protected:
    bool compiled {};
    stream::mesh mesh {};
public:
    buffering buffer {};
    bool static_buffers {true};
    util::aabb aabb {};
public:
    explicit model(stream::mesh &_mesh);
    ~model();

    void load(stream::mesh &_mesh);
    void recompile();
    bool is_compiled();
};

#endif