#include "bp3/TypeInfo.hpp"

#if __GNUC__
#include <cxxabi.h>
#define BP3_GCC_DEMANGLE
#endif

namespace bp3 {

#ifdef BP3_GCC_DEMANGLE

std::string TypeInfo::demangle() const {
    int status = 0;
    char * s = abi::__cxa_demangle(name(), nullptr, nullptr, &status);
    std::string r;
    if (status == 0) {
        r = s;
    } else {
        r = name();
        r += " (mangled)";
    }
    free(s);
    return r;
}

#else

std::string TypeInfo::demangle() const { return name(); }

#endif

} // namespace bp3
