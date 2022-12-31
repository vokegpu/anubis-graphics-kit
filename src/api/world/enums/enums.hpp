#ifndef API_WORLD_ENUMS_H
#define API_WORLD_ENUMS_H

namespace enums {
	enum class material {
		empty, light, metal, dialetric
	};

	enum class state {
		enable, disable
	};

    enum class type {
        abstract, entity, object, model
    };
};

#endif