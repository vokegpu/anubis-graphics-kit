#ifndef AGK_ASSET_MANAGER_H
#define AGK_ASSET_MANAGER_H

#include <iostream>
#include <map>
#include <unordered_map>
#include "core/imodule.hpp"

class assetmanager : public imodule {
private:
    void do_create_asset(imodule *&p_asset);
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