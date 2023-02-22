#include "asset_manager.hpp"
#include "util/env.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "model.hpp"

void asset_manager::load(imodule *p_asset) {
    this->asset_map[p_asset->tag] = p_asset;
}

imodule *asset_manager::find(std::string_view resource) {
    return this->asset_map[resource.data()];
}

void asset_manager::on_create() {
    imodule::on_create();

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

    this->load(new asset::texture<uint8_t> {"texture/terrain.atlas", "./data/textures/terrain_stone.png", {GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_RGB, GL_RGB}, [](gpu::texture &texture, bool &mipmap) {
        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTextureParameteri(texture.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(texture.type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(texture.type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        mipmap = true;
    }});

    this->load(new asset::model {"model/simple-tree", "./data/models/tree-simple.obj", {GL_STATIC_DRAW, GL_STATIC_DRAW, GL_STATIC_DRAW, GL_STATIC_DRAW}});

    util::log("Loading all assets");
    std::string asset_message {};

    for (auto &[key, value] : this->asset_map) {
        value->on_create();

        asset_message.clear();
        asset_message += "Assets '";
        asset_message += key;
        asset_message += "' ";

        if (value->is_dead) {
            asset_message += "failed to be loaded";
        } else {
            asset_message += "loaded";
        }

        util::log(asset_message);
    }
}

void asset_manager::on_destroy() {
}