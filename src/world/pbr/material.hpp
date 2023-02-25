#ifndef AGK_WORLD_PBR_MATERIAL_H
#define AGK_WORLD_PBR_MATERIAL_H

#include <glm/glm.hpp>
#include "world/enums/enums.hpp"
#include "asset/texture.hpp"
#include "asset/model.hpp"
#include <vector>

class material {
protected:
    glm::vec3 color {};
    float rough {0.43f};
    enums::material type {enums::material::empty};
    std::vector<uint32_t> texture_list {};
    bool material_mtl_updated {};
    std::string asset_model_mtl {};
public:
    explicit material(enums::material material_type);
    ~material();

    template<typename t>
    void append_texture(::asset::texture<t> *p_asset_texture) {
        if (p_asset_texture == nullptr) {
            return;
        }

        this->append_texture(p_asset_texture->gpu_side_data().id);
    }

    void append_mtl(::asset::model *p_asset_model);
    void append_texture(uint32_t texture);
    void remove_texture(uint32_t texture);
    bool invoke_all_textures();

    void set_color(glm::vec3 color3f);
    glm::vec3 get_color();

    void set_rough_based_type(enums::material material_type);
    void set_rough(float rough_value);
    float get_rough();

    enums::material get_type();
};

#endif