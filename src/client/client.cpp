#include "client.hpp"
#include "agk.hpp"
#include "client/scenes/starter.hpp"
#include "client/services/camera_manager.hpp"

int32_t main(int32_t argv, char** argc) {
    auto *p_scene {new client::scenes::starter()};
    p_scene->p_camera_manager = new client::services::camera_manager();

    agk::path(argc[0]);
    agk::task::registry(p_scene->p_camera_manager);
    agk::mainloop(p_scene);

    return 0;
}