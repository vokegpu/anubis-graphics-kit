#ifndef AGK_API_ASSET_IMPL_MODEL_H
#define AGK_API_ASSET_IMPL_MODEL_H

#include "core/imodule.hpp"
#include "mesh.hpp"
#include "gpu/tools.hpp"
#include "util/math.hpp"
#include <functional>
#include <iostream>

namespace asset {
    class model : public imodule {
    protected:
        std::string path {};
        std::function<void(buffering&, ::mesh::data&)> mixin {};
        uint32_t driver_mode[4] {};
    public:
        util::aabb axis_aligned_bounding_box {};
        buffering buffer {};

        model(std::string_view model_tag, std::string_view model_path, const glm::ivec4 &model_driver_mode, const std::function<void(buffering&, ::mesh::data&)> &injection_mixin = {});
        ~model();

        void on_create() override;
    };
}

#endif