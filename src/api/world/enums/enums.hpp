#ifndef API_WORLD_ENUMS_H
#define API_WORLD_ENUMS_H

namespace enums {
	enum class material {
		empty, metal, dialetric
	};

    /*
     * Invocation priority.
     * low: Callable in specific ticks.
     * high: Update all framerate ticks.
     */
    enum class priority {
        low, high
    };

	enum class state {
		enable, disable
	};

    enum class type {
        abstract, entity, object, light, model
    };
};

#endif