#include "api/asset/manager/asset_manager.hpp"
#include "api/util/env.hpp"
#include "api/asset/impl/shader.hpp"
#include "api/asset/impl/texture.hpp"

void asset_manager::load(feature *p_feature) {
    this->asset_map[p_feature->tag] = p_feature;
}

feature *asset_manager::find(std::string_view resource) {
    return this->asset_map[resource.data()];
}

void asset_manager::on_create() {
    feature::on_create();

    this->load(new asset::shader {"gpu/effects.coordinate.debug", {
            {"./data/effects/coordinate.debug.vert", GL_VERTEX_SHADER},
            {"./data/effects/coordinate.debug.frag", GL_FRAGMENT_SHADER}
    }});

    this->load(new asset::shader {"gpu/effects.material.brdf.pbr", {
            {"./data/effects/material.brdf.pbr.vert", GL_VERTEX_SHADER},
            {"./data/effects/material.brdf.pbr.frag", GL_FRAGMENT_SHADER}
    }});

    this->load(new asset::shader {"gpu/effects.processing.post", {
            {"./data/effects/processing.post.vert", GL_VERTEX_SHADER},
            {"./data/effects/processing.post.frag", GL_FRAGMENT_SHADER}
    }});

    this->load(new asset::shader {"gpu/effects.terrain.pbr", {
            {"./data/effects/terrain.pbr.vert", GL_VERTEX_SHADER},
            {"./data/effects/terrain.pbr.frag", GL_FRAGMENT_SHADER},
            {"./data/effects/terrain.pbr.tesc", GL_TESS_CONTROL_SHADER},
            {"./data/effects/terrain.pbr.tese", GL_TESS_EVALUATION_SHADER}
    }});

    this->load(new asset::shader {"gpu/scripts.cloud.generator.script", {
            {"./data/scripts/cloud.generator.comp", GL_COMPUTE_SHADER}
    }});

    this->load(new asset::shader {"gpu/scripts.hdr.luminance", {
            {"./data/scripts/hdr.luminance.comp", GL_COMPUTE_SHADER}
    }});

    this->load(new asset::shader {"gpu/scripts.heightmap.generator", {
            {"./data/scripts/heightmap.generator.comp", GL_COMPUTE_SHADER}
    }});

    this->load(new asset::texture<uint8_t> {"texture/terrain.atlas", "./data/textures/terrain_stone.png", GL_UNSIGNED_INT, [](gpu::texture &texture, bool &mipmap) {
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        mipmap = true;
    }});

    util::log("Loading assets...");
    std::string asset_message {};

    for (auto &[key, value] : this->asset_map) {
        value->on_create();

        asset_message.clear();
        asset_message += "Assets '";
        asset_message += key;
        asset_message += " loaded!";
        util::log(asset_message);
    }
}

void asset_manager::on_destroy() {
    feature::on_destroy();
}