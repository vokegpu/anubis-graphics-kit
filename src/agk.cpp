#include "agk.hpp"
#include <iostream>

agk_client* const the_agk_core = new agk_client();

int main(int argv, char** argc) {
    the_agk_core->init();
    the_agk_core->run();
    the_agk_core->shutdown();

    return 0;
}