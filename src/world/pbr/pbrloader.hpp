#ifndef AGK_WORLD_PBR_LOADER_H
#define AGK_WORLD_PBR_LOADER_H

#include "material.hpp"
#include "model.hpp"
#include <map>
#include <vector>
#include <unordered_map>
#include "world/environment/object.hpp"

class pbrloader : public imodule {
private:
    static std::vector<std::string> dontcare;
    std::unordered_map<std::string, imodule*> pbr_map {};
    std::map<std::string, uint32_t> pbr_family_map {};
protected:
    std::vector<model*> uncompiled_model_list {};
    int32_t find_equals_material(std::map<std::string, std::string> &metadata);
    int32_t extract_family_tag(std::string_view pbr, std::string &family_tag);
    void direct_assign_object(object *p_object, std::string_view model, std::string_view object);
public:
public:
    bool load_material(std::string_view tag, material *p_material);
    bool load_model(std::string_view tag, std::string_view path);
    bool load_material(std::string_view tag, std::string_view path);

    imodule *find(std::string_view pbr_tag);
    uint32_t find_family(std::string_view pbr_tag);

    void assign_object(object *p_object, std::string_view model, std::string_view object);
    void on_update() override;
};

#endif