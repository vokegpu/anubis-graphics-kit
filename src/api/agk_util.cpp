#include "agk_util.hpp"

void util::log(const std::string &name) {
    const std::string property = "[MAIN] " + name;
    std::cout << property.c_str() << "\n";
}
