#ifndef AGK_WORLD_ENVIRONMENT_OBJECT_TRANSFORM_H
#define AGK_WORLD_ENVIRONMENT_OBJECT_TRANSFORM_H

#include <glm/glm.hpp>

struct objectransform {
public:
    glm::mat4 mat4x4 {};
    glm::vec3 position {};
    glm::vec3 scale {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation {};
public:
    void set_position(float x, float y, float z);
    void set_scale(float x, float y, float z);
};

#endif

#ifndef AGK_WORLD_ENVIRONMENT_OBJECT_RENDERING_PAIR_H
#define AGK_WORLD_ENVIRONMENT_OBJECT_RENDERING_PAIR_H

struct renderingpair {
    model *p_linked_model {};
    material *p_linked_material {};
};

#endif

#ifndef AGK_WORLD_ENVIRONMENT_OBJECT_H
#define AGK_WORLD_ENVIRONMENT_OBJECT_H

#include "world/enums/enums.hpp"
#include "world/pbr/material.hpp"
#include "world/pbr/model.hpp"

class object : public imodule {
protected:
    std::vector<renderingpair> rendering_pair_list {};
    enums::state visible {};
    enums::priority priority {};
public:
    std::vector<std::string> model_group {};
    std::vector<std::string> material_group {};

    enums::type type {};
    buffering *p_instance {};
    objectransform transform {};
    util::aabb aabb {};
public:
    explicit object(material *p_linked_material = nullptr);
    ~object();

    void set_visible(enums::state enum_state, bool dispatch_event = true);
    enums::state get_visible();

    void set_priority(enums::priority enum_priority, bool dispatch_event = true);
    enums::priority get_priority();

    virtual void on_low_update();
    void on_render() override;
};

#endif