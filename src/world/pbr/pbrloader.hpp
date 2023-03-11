#ifndef AGK_WORLD_PBR_LOADER_H
#define AGK_WORLD_PBR_LOADER_H

#include "material.hpp"
#include "model.hpp"
#include <map>
#include <vector>
#include <unordered_map>

class pbrloader {
private:
	std::unordered_map<std::string, uint32_t> material_map {};
	std::unordered_map<std::string, model*> model_maps {};
	std::vector<material*> material_list {};
protected:
	int32_t find_equals_material(std::map<std::string, std::string> &metadata);
public:
	bool load_material(std::vector<std::string> &material_list, std::string_view path);

	void registry(material *p_material);
	void registry(model *p_model);

	material *find_material(std::string_view k_name);
	model *find_model(std::string_view k_name);
};

#endif