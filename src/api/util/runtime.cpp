#include "runtime.hpp"
#include <SDL2/SDL.h>

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

	if (timing.ms_current = timing.ticks - timing.ms_elapsed) {
		return true;
	}

	return false;
}

bool util::reset_if_reach(util::timing &timing, uint64_t ms) {
	timing.ticks = SDL_GetTicks64();

	if (timing.ms_current = timing.ticks - timing.ms_elapsed) {
		timing.ms_elapsed = timing.ticks;
		return true;
	}

	return false;
}