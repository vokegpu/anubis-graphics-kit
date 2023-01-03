#ifndef AGK_API_GC_H
#define AGK_API_GC_H

#include "api/feature/feature.hpp"
#include <queue>

class gc {
protected:
    std::queue<feature*> queue {};
public:
    bool should_poll_uncreated {};
    bool should_poll {};

    void create(feature *p_feature);
    void destroy(feature *p_feature);
    void do_update();
};

#endif