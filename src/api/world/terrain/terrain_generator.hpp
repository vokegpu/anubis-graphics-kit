#ifndef AGK_API_WORLD_TERRAIN_GENERATOR_H
#define AGK_API_WORLD_TERRAIN_GENERATOR_H

#include "terrain.hpp"
#include <iostream>

class terrain_generator {
protected:
    int32_t token {1};
public:
    void init();
    terrain *gen_terrain();
};

#endif