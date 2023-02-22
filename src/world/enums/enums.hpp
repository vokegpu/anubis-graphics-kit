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
        none, low, high
    };

	enum class state {
        none, enable, disable
	};

    enum class type {
        abstract, entity, object, light, model
    };
};

#endif