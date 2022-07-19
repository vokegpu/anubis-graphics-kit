#include "agk_client.hpp"
#include <iostream>

agk_core* const the_agk_core = new agk_core();

int main(int argv, char** argc) {
    the_agk_core->init();
    the_agk_core->run();
    the_agk_core->shutdown();

    return 0;
}