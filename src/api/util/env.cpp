#include "env.hpp"
#include <SDL2/SDL.h>
#include <sstream>
#include <string>

void util::log(const std::string &log_message) {
	const std::string full_log_message {"[AGK] " + log_message};
	std::cout << full_log_message.c_str()<< '\n';
}

bool util::reset(util::timing &timing) {
	timing.ticks = SDL_GetTicks64();
	timing.ms_elapsed = timing.ticks;
	return true;
}

bool util::reach(util::timing &timing, uint64_t ms) {
	timing.ticks = SDL_GetTicks64();

	if ((timing.ms_current = timing.ticks - timing.ms_elapsed) > ms) {
		return true;
	}

	return false;
}

bool util::resetifreach(util::timing &timing, uint64_t ms) {
	timing.ticks = SDL_GetTicks64();

	if ((timing.ms_current = timing.ticks - timing.ms_elapsed) > ms) {
		timing.ms_elapsed = timing.ticks;
		return true;
	}

	return false;
}

void util::split(std::vector<std::string> &list, std::string_view str, const char divisor) {
	std::string string_buffer {};
	std::stringstream ss(str.data());
	list.clear();

	while (std::getline(ss, string_buffer, divisor)) {
		list.push_back(string_buffer);
	}
}