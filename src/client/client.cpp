#include "client.hpp"
#include "api/api.hpp"
#include "client/scenes/starter.hpp"

int32_t main(int32_t argv, char** argc) {
    api::path(argc[0]);
    api::mainloop(new client::scenes::starter());
    return 0;
}