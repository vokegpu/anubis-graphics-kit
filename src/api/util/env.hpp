#ifndef AGK_UTIL_RUNTIME_H
#define AGK_UTIL_RUNTIME_H

#include <vector>
#include <iostream>

namespace util {
    void log(const std::string &log_message);

    struct timing {
        uint64_t ms_elapsed {};
        uint64_t ms_current {};
        uint64_t ticks {};
    };

    bool reset(util::timing&);
    bool reach(util::timing&, uint64_t);
    bool resetifreach(util::timing&, uint64_t);

    void split(std::vector<std::string>&, std::string_view, const char);

    struct texture {
        std::string path {};
        unsigned char *p_data {};
        uint32_t w {}, h {};
        bool loaded {};
    };

    bool loadtexture(texture *p_texture);
    uint32_t createtexture(texture *p_texture);

}

#endif