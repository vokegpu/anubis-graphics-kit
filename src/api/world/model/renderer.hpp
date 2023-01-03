#ifndef AGK_API_WORLD_MODEL_RENDERER_H
#define AGK_API_WORLD_MODEL_RENDERER_H

#include "model.hpp"
#include "api/mesh/mesh.hpp"
#include <vector>
#include <map>

class renderer {
private:
    std::vector<model*> loaded_model_list {};
    std::map<std::string, int32_t> model_register_map {};
public:
    model *add(std::string_view tag, mesh::data &mesh_data);
    bool find(std::string_view tag, model *& p_model);
    bool contains(std::string_view tag);

};

#endif