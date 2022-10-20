#ifndef AGK_API_H
#define AGK_API_H

#include "api/core/core.hpp"

namespace api {
	extern core app;
	void mainloop(feature*);

	namespace scene {
		void load(feature*);
		feature* &current();
	}

	namespace gc {
		void destroy(feature*);
		void create(feature*);
	}
};

#endif