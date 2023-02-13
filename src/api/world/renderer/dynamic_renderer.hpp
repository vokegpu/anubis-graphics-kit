#ifndef AGK_WORLD_RENDERER_DYNAMIC_INSTANCED_GEOMETRY_H
#define AGK_WORLD_RENDERER_DYNAMIC_INSTANCED_GEOMETRY_H

#include "api/gpu/tools.hpp"
#include "api/feature/feature.hpp"

class dynamic_geometry_instanced_renderer : public feature {
public:
    std::map<int32_t, buffering> buffer_map {};

    bool find(int32_t id, buffering &buffer);
    buffering &registry(int32_t id);
    void unregister(int32_t id);
};

#endif