#ifndef AGK_WORLD_ENVIRONMENT_OBJECT_TRANSFORM_H
#define AGK_WORLD_ENVIRONMENT_OBJECT_TRANSFORM_H

#include <glm/glm.hpp>

struct object_transform {
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

#ifndef AGK_WORLD_ENVIRONMENT_OBJECT_H
#define AGK_WORLD_ENVIRONMENT_OBJECT_H

#include "core/imodule.hpp"
#include "world/enums/enums.hpp"
#include "world/pbr/material.hpp"
#include "asset/model.hpp"

class object : public imodule {
protected:
    enums::state visible {enums::state::none};
    enums::priority priority {enums::priority::none};
public:
    explicit object(material *p_linked_material = nullptr);
    ~object();

    std::vector<asset::model::child> model_children {};
    asset::model *p_model {};

    enums::type type {enums::type::object};
    buffering *p_instance {};
    object_transform transform {};
    util::aabb aabb {};

    void set_material(material *p_material);
    void assign_material(std::string_view name, material *p_material);

    void set_visible(enums::state enum_state, bool dispatch_event = true);
    enums::state get_visible();

    void set_priority(enums::priority enum_priority, bool dispatch_event = true);
    enums::priority get_priority();

    virtual void on_low_update();
    void on_render() override;
};

#endif