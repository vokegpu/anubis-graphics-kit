#ifndef AGK_ASSET_MODEL_H
#define AGK_ASSET_MODEL_H

#include "core/imodule.hpp"
#include "mesh.hpp"
#include "gpu/tools.hpp"
#include "util/math.hpp"
#include <functional>
#include <iostream>

namespace asset {
    class model : public imodule {
    protected:
        std::string path {}, name {};
        std::function<void(buffering&, ::mesh::data&)> mixin {};
        uint32_t driver_mode[4] {};
        std::vector<std::string> linked_mtl_list {};
    public:
        util::aabb aabb {};
        buffering buffer {};
        ::mesh::wavefront mtllib_wavefront {};

        explicit model() = default;
        model(std::string_view model_tag, std::string_view model_path, const glm::ivec4 &model_driver_mode, const std::function<void(buffering&, ::mesh::data&)> &injection_mixin = {});
        model(std::string_view model_tag, const std::function<void(buffering&, ::mesh::data&)> &injection_mixin = {});
        ~model();

        std::vector<std::string> &get_linked_mtl_list();
        void on_create() override;
    };
}

#endif