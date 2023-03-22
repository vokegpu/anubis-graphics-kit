#include "core.hpp"

std::vector<imodule*> core::renderablelist {};
std::vector<imodule*> core::updateablelist {};
std::vector<imodule*> core::listenablelist {};
int32_t core::token {};
std::queue<imodule*> core::taskqueue {};
std::string core::version {"0.4.0"};