#include "client.hpp"
#include "api/api.hpp"
#include "client/scenes/starter.hpp"
#include "client/services/camera_manager.hpp"

int32_t main(int32_t argv, char** argc) {
    api::path(argc[0]);
    api::service::registry(new client::services::camera_manager());
    api::mainloop(new client::scenes::starter());
    return 0;
}