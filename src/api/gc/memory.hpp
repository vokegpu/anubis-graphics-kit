#ifndef API_GC_CPU_RAM_H
#define API_GC_CPU_RAM_H

#include <map>
#include <iostream>

struct memory {
protected:
	static std::map<int32_t, void*> map;
public:
	static void *check(int32_t id);
	static void emmite(int32_t, void*);
	static void erase(int32_t);
};

#endif