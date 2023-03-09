#include "assetmanager.hpp"
#include "util/env.hpp"
#include "shader.hpp"
#include "texture.hpp"

void assetmanager::do_create_asset(imodule *&p_asset) {
    p_asset->on_create();

    std::string msg {"Assets '"};
    msg += reload_asset_list[it];
    msg += "' ";

    if (p_asset->is_dead) {
        msg += "failed to be loaded";
    } else {
        msg += "loaded";
    }

    util::log(msg);
}

void assetmanager::load(imodule *p_asset) {
    this->asset_map[p_asset->tag] = p_asset;
    this->reload_asset_list.push_back(p_asset->tag);
}

imodule *assetmanager::find(std::string_view resource) {
    return this->asset_map[resource.data()];
}

void assetmanager::on_create() {
    imodule::on_create();

    this->load(new asset::shader {"effects.coordinate.debug", {
            {"./data/effects/coordinate.debug.vert", GL_VERTEX_SHADER},
            {"./data/effects/coordinate.debug.frag", GL_FRAGMENT_SHADER}
    }});

    this->load(new asset::shader {"effects.material.brdf.pbr", {
            {"./data/effects/material.brdf.pbr.vert", GL_VERTEX_SHADER},
            {"./data/effects/material.brdf.pbr.frag", GL_FRAGMENT_SHADER}
    }, [](asset::shader *p_shader) {
        float empty_buffer[12] {};
        p_shader->attach("uniformBufferMaterial", 0);
        p_shader->shaderbuffer.invoke(0, GL_UNIFORM_BUFFER);
        p_shader->shaderbuffer.send<float>(sizeof(empty_buffer) * 512, nullptr, GL_DYNAMIC_DRAW);
        p_shader->shaderbuffer.bind(0);
        p_shader->shaderbuffer.revoke();
    }});

    this->load(new asset::shader {"effects.processing.post", {
            {"./data/effects/processing.post.vert", GL_VERTEX_SHADER},
            {"./data/effects/processing.post.frag", GL_FRAGMENT_SHADER}
    }});

    this->load(new asset::shader {"effects.terrain.pbr", {
            {"./data/effects/terrain.pbr.vert", GL_VERTEX_SHADER},
            {"./data/effects/terrain.pbr.frag", GL_FRAGMENT_SHADER},
            {"./data/effects/terrain.pbr.tesc", GL_TESS_CONTROL_SHADER},
            {"./data/effects/terrain.pbr.tese", GL_TESS_EVALUATION_SHADER}
    }});

    this->load(new asset::shader {"scripts.cloud.generator.script", {
            {"./data/scripts/cloud.generator.comp", GL_COMPUTE_SHADER}
    }});

    this->load(new asset::shader {"scripts.hdr.luminance", {
            {"./data/scripts/hdr.luminance.comp", GL_COMPUTE_SHADER}
    }});

    this->load(new asset::shader {"scripts.heightmap.generator", {
            {"./data/scripts/heightmap.generator.comp", GL_COMPUTE_SHADER}
    }});

    this->load(new asset::texture<uint8_t> {"terrain.atlas", "./data/textures/terrain_atlas.png", {GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_RGB, GL_RGB}, [](gpu::texture &texture, bool &mipmap) {
        glTextureParameteri(texture.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(texture.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTextureParameteri(texture.type, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texture.type, GL_TEXTURE_WRAP_T, GL_REPEAT);

        mipmap = true;
    }});

    /* flush default assets */
    this->on_update();
}

void assetmanager::on_destroy() {
}

void assetmanager::on_update() {
    if (this->reload_asset_list.empty()) {
        return;
    }

    util::log("Attempting to reload new assets");
    std::string asset_message {};
    int32_t extra_asset_list {};

    for (const std::string &assets : this->reload_asset_list) {
        this->do_create_asset(this->asset_map[assets]);
        extra_asset_list++;
    }

    util::log("Assets size: " + std::to_string(this->reload_asset_list.size()));
    util::log("Assets reloaded: " + std::to_string(extra_asset_list));

    for (int32_t it {extra_asset_list}; it < this->reload_asset_list.size(); it++) {
        this->do_create_asset(this->asset_map[this->reload_asset_list[it]]);
    }

    this->reload_asset_list.clear();
}