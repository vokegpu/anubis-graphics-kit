#include "memory.hpp"

std::vector<int32_t, void*> memory::map {};

void *memory::check(int32_t id) {
	return memory::map[id];
}

void memory::emmite(int32_t id, void *ptr) {
	memory::map[id] = ptr;
}

void memory::erase(int32_t id) {
	// todo: map erase item.
}