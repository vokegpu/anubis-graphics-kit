#ifndef AGK_API_UTIL_FILE_H
#define AGK_API_UTIL_FILE_H

#include <iostream>

namespace util {
    bool readfile(std::string_view, std::string&);
}

#endif