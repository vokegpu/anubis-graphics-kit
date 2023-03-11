#ifndef AGK_WORLD_PBR_MATERIAL_H
#define AGK_WORLD_PBR_MATERIAL_H

#include <glm/glm.hpp>
#include "world/enums/enums.hpp"
#include "asset/texture.hpp"
#include <vector>

class material : public imodule {
private:
    static int32_t token;
    material *p_material_linked {};
protected:
    std::map<std::string, std::string> metadata {};
    std::map<std::string, uint32_t> sampler_map {};

    bool double_sided {};
    bool should_reload {};
    bool should_reload_textures {};

    int32_t id {};
    int32_t shader_index {};
public:
    material(std::map<std::string, std::string> &_metadata);
    ~material() = default;

    inline uint32_t &operator[](std::string_view k_sampler) {
        return this->sampler_map[k_sampler.data()];
    }

    std::map<std::string, std::string> &get_metadata(); 
    void invoke(::asset::shader *p_shader);
};

#endif