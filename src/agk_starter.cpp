#include "agk_starter.hpp"
#include <iostream>

agk* const the_agk_core = new agk();

int main(int argv, char** argc) {
    the_agk_core->init();
    the_agk_core->run();
    the_agk_core->shutdown();

    return 0;
}