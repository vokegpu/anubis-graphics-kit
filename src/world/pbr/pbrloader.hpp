#ifndef AGK_WORLD_PBR_LOADER_H
#define AGK_WORLD_PBR_LOADER_H

#include "material.hpp"
#include "model.hpp"
#include <map>
#include <vector>
#include <unordered_map>

class pbrloader : public imodule {
private:
	std::unordered_map<std::string, material*> material_map {};
	std::unordered_map<std::string, model*> model_map {};
protected:
	std::vector<model*> uncompiled_model_list {};
	int32_t find_equals_material(std::map<std::string, std::string> &metadata);
public:
	static std::vector<std::string> dontcare;
public:
	bool load_model(std::string_view name, std::string<std::string> &loaded_model_list, std::string_view path);
	bool load_material(std::vector<std::string> &loaded_material_list, std::string_view path);

	material *&find_material(std::string_view k_name);
	model *&find_model(std::string_view k_name);

	void on_update() override;
};

#endif