#ifndef AGK_WORLD_PBR_MATERIAL_METADATA_H
#define AGK_WORLD_PBR_MATERIAL_METADATA_H

#include <glm/glm.hpp>

struct material_metadata {
    glm::vec3 color {};
    float rough {};

    int32_t metal {};
    int32_t active_sampler_albedo {};
    int32_t active_sampler_specular {};
    int32_t active_sampler_normal_map {};
};

#endif

#ifndef AGK_WORLD_PBR_MATERIAL_H
#define AGK_WORLD_PBR_MATERIAL_H

#include <glm/glm.hpp>
#include "world/enums/enums.hpp"
#include "asset/texture.hpp"
#include "asset/model.hpp"
#include <vector>

class material {
private:
    static int32_t token;
protected:
    std::map<std::string, uint32_t> sampler_map {};
    bool should_reload {};
    int32_t id {};
    int32_t shader_index {};
    material_metadata metadata {};
public:
    asset::model *p_model {};

    material();
    ~material();

    uint32_t &operator[](std::string_view k_sampler) {
        return this->sampler_map[k_sampler.data()];
    }

    material_metadata &getmetadata();

    void set_is_metal(bool _is_metal);
    bool is_metal();

    void set_rough(float _rough);
    float get_rough();

    void set_color(const glm::vec3 &color);
    const glm::vec3 &get_color();

    void invoke(::asset::shader *p_shader);
};

#endif