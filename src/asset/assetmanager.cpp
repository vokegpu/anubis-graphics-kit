#include "assetmanager.hpp"
#include "util/env.hpp"
#include "shader.hpp"
#include "texture.hpp"

void assetmanager::do_create_asset(imodule *&p_asset) {
    p_asset->on_create();

    std::string msg {"Assets '"};
    msg += p_asset->tag;
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
            {"./data/gpu/coordinate.debug.vert", GL_VERTEX_SHADER},
            {"./data/gpu/coordinate.debug.frag", GL_FRAGMENT_SHADER}
    }});

    this->load(new asset::shader {"effects.pbr.material", {
            {"./data/gpu/pbr.material.vert", GL_VERTEX_SHADER},
            {"./data/gpu/pbr.material.frag", GL_FRAGMENT_SHADER}
    }, [](asset::shader *p_shader) {
        float empty_buffer_material[12] {};
        float empty_buffer_lighting[8] {};

        p_shader->attach("uniformBufferMaterial", 0);
        p_shader->programbuffer.invoke(0, GL_UNIFORM_BUFFER);
        p_shader->programbuffer.send<float>((sizeof(empty_buffer_material) * 512) +
                                            (sizeof(empty_buffer_lighting) * 64), nullptr, GL_DYNAMIC_DRAW);
        p_shader->programbuffer.bind(0);
        p_shader->programbuffer.revoke();
    }});

    this->load(new asset::shader {"effects.processing.post", {
            {"./data/gpu/processing.post.vert", GL_VERTEX_SHADER},
            {"./data/gpu/processing.post.frag", GL_FRAGMENT_SHADER}
    }});

    this->load(new asset::shader {"effects.sky.pbr", {
        {"./data/gpu/sky.pbr.vert", GL_VERTEX_SHADER},
        {"./data/gpu/sky.pbr.frag", GL_FRAGMENT_SHADER}
    }});

    this->load(new asset::shader {"scripts.cloud.generator.script", {
            {"./data/gpu/cloud.generator.comp", GL_COMPUTE_SHADER}
    }});

    this->load(new asset::shader {"scripts.hdr.luminance", {
            {"./data/gpu/hdr.luminance.comp", GL_COMPUTE_SHADER}
    }});

    this->load(new asset::shader {"scripts.heightmap.generator", {
            {"./data/gpu/heightmap.generator.comp", GL_COMPUTE_SHADER}
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