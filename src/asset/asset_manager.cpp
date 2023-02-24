#include "asset_manager.hpp"
#include "util/env.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "model.hpp"

void asset_manager::load(imodule *p_asset) {
    this->asset_map[p_asset->tag] = p_asset;
    this->reload_asset_list.push_back(p_asset->tag);
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
        glTextureParameteri(texture.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(texture.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTextureParameteri(texture.type, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texture.type, GL_TEXTURE_WRAP_T, GL_REPEAT);

        mipmap = true;
    }});

    this->load(new asset::model {"models/simple-tree", "./data/models/tree-simple.obj", glm::ivec4(GL_STATIC_DRAW), [](buffering &buffer, ::mesh::data &mesh) {
        buffer.primitive[0] = GL_TRIANGLE_STRIP;
    }});

    this->on_update();
}

void asset_manager::on_destroy() {
}

void asset_manager::on_update() {
    if (this->reload_asset_list.empty()) {
        return;
    }

    util::log("Attempting to reload new assets");
    std::string asset_message {};
    imodule *p_asset {};

    for (const std::string &assets : this->reload_asset_list) {
        p_asset = this->asset_map[assets];
        p_asset->on_create();

        asset_message.clear();
        asset_message += "Assets '";
        asset_message += assets;
        asset_message += "' ";

        if (p_asset->is_dead) {
            asset_message += "failed to be loaded";
        } else {
            asset_message += "loaded";
        }

        util::log(asset_message);
    }

    this->reload_asset_list.clear();
}
