#include "agk.hpp"
#include "client/scenes/starter.hpp"

int32_t main(int32_t, char**) {
    auto *p_scene {new client::scenes::starter()};

    agk::path(argc[0]);
    agk::mainloop(p_scene)
    return 0;
}