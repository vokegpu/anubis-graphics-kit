#include "client.hpp"
#include "api/api.hpp"
#include "client/scenes/starter.hpp"
#include "client/services/camera_manager.hpp"

int32_t main(int32_t argv, char** argc) {
    auto p_scene {new client::scenes::starter()};
    p_scene->p_camera_manager = new client::services::camera_manager();

    api::path(argc[0]);
    api::task::registry(p_scene->p_camera_manager);
    api::mainloop(p_scene);
    return 0;
}