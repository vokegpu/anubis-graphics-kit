#ifndef AGK_WORLD_ENVIRONMENT_OBJECT_TRANSFORM_H
#define AGK_WORLD_ENVIRONMENT_OBJECT_TRANSFORM_H

#include <glm/glm.hpp>

struct objecttransform {
public:
    glm::mat4 mat4x4 {};
    glm::vec3 position {};
    glm::vec3 scale {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation {};
private:
    void set_position(float x, float y, float z);
    void set_scale(float x, float y, float z);
};

#endif

#ifndef AGK_WORLD_ENVIRONMENT_OBJECT_RENDER_H
#define AGK_WORLD_ENVIRONMENT_OBJECT_RENDER_H

struct objectrender {
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
    enums::state visible {};
    enums::priority priority {};
public:
    std::map<std::string, objectrender> object_render_map {};
    std::vector<objectrender*> ref_object_render_list {};
public:
    enums::type type {};
    buffering *p_instance {};
    objecttransform transform {};
    util::aabb aabb {};
public:
    explicit object();
    ~object();

    void append();    

    void append(scene::string_view) {}
    void append(scene::string_view);
    void append(model *p_model);
    void append(std::string_view model, std::string_view material);

    void set_visible(enums::state enum_state, bool dispatch_event = true);
    enums::state get_visible();

    void set_priority(enums::priority enum_priority, bool dispatch_event = true);
    enums::priority get_priority();

    virtual void on_low_update();
};

#endif