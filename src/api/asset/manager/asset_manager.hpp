#ifndef AGK_API_ASSET_MANAGER_H
#define AGK_API_ASSET_MANAGER_H

#include <iostream>
#include <map>
#include <unordered_map>
#include "api/feature/feature.hpp"

class asset_manager : public feature {
protected:
    std::unordered_map<std::string, feature*> asset_map {};
public:
    void load(feature *p_feature);
    feature *find(std::string_view);

    void on_create() override;
    void on_destroy() override;
};

#endif