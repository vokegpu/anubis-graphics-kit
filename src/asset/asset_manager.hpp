#ifndef AGK_ASSET_MANAGER_H
#define AGK_ASSET_MANAGER_H

#include <iostream>
#include <map>
#include <unordered_map>
#include "core/imodule.hpp"

class asset_manager : public imodule {
protected:
    std::unordered_map<std::string, imodule*> asset_map {};
    std::vector<std::string> reload_asset_list {};
public:
    void load(imodule *p_asset);
    imodule *find(std::string_view);

    void on_create() override;
    void on_update() override;
    void on_destroy() override;
};

#endif