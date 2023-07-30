#ifndef AGK_WORLD_PBR_MODEL_H
#define AGK_WORLD_PBR_MODEL_H

#include "gpu/gpubuffer.hpp"
#include "core/imodule.hpp"
#include <functional>
#include "util/math.hpp"

class model : public imodule {
protected:
    bool compiled {};
public:
    buffering buffer {};
    bool static_buffers {true};
    util::aabb aabb {};
    stream::mesh mesh {};
public:
    explicit model(stream::mesh &_mesh);
    ~model();

    stream::mesh &get_mesh();
    void load(stream::mesh &_mesh);
    void recompile();
    bool is_compiled();
};

#endif