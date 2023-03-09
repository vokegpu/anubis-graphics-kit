#ifndef AGK_WORLD_PBR_LOADER_H
#define AGK_WORLD_PBR_LOADER_H

#include "material.hpp"
#include "model.hpp"

class pbrloader {
public:
	std::vector<material*> material_list {};
	std::vector<model*> model_list {};
}

#endif