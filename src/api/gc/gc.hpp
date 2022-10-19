#ifndef AGK_GC_H
#define AGK_GC_H

#include "api/core/feature.hpp"
#include <queue>

class gc {
protected:
	static std::queue<feature*> queue_uncreated {};
	static std::queue<feature*> queue_undestroyed {};
public:
	bool should_poll_uncreated {};
	bool should_poll_undestroyed {};

	void create(feature*);
	void destroy(feature*);
	void do_update();
};

#endif