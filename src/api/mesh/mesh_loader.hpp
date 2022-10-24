#ifndef AGK_API_MESH_LOADER_H
#define AGK_API_MESH_LOADER_H

#include <string>
#include "mesh.hpp"

class mesh_loader {
public:
    bool load_object(mesh::data&, std::string_view);
};

#endif
