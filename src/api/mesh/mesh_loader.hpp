#ifndef AGK_API_MESH_LOADER_H
#define AGK_API_MESH_LOADER_H

#include <string>
#include "mesh.hpp"

class mesh_loader {
public:
    void process_indexing_sequence(std::vector<uint32_t> &indexes, std::vector<float> &data);
    bool load_object(mesh::data&, std::string_view);
};

#endif
