#ifndef BP3_type_id_hpp_INCLUDED
#define BP3_type_id_hpp_INCLUDED

#include "bp3/py_ptr.hpp"

#include <typeinfo>
#include <cstring>

namespace bp3 {

// Essentially copied from Boost.Python v2 implementation
// (but with fewer workarounds for old compilers that we
// know we can't support anymore for other reasons).
// This provides a copyable type_info class that works
// across shared library boundaries.

#if (defined(__GNUC__) && __GNUC__ >= 3)       \
 || defined(_AIX) \
 || (   defined(__sgi) && defined(__host_mips)) \
 || (defined(__hpux) && defined(__HP_aCC)) \
 || (defined(linux) && defined(__INTEL_COMPILER) && defined(__ICC))
#define BP3_TYPE_ID_NAME
#endif 

struct type_info {

    inline type_info(std::type_info const & = typeid(void));

    inline bool operator==(type_info const & other) const;

    char const * name() const;

 private:
#ifdef BP3_TYPE_ID_NAME
    typedef char const* base_id_t;
#else
    typedef std::type_info const* base_id_t;
#endif
    base_id_t _base_type;
};

inline type_info::type_info(std::type_info const & t)
    : _base_type(
#ifdef BP3_TYPE_ID_NAME
        t.name()
#else
        &t
#endif
    ) {}

template <typename T>
inline type_info type_id() { return type_info(typeid(T)); }

inline bool type_info::operator==(type_info const & other) const {
#ifdef BP3_TYPE_ID_NAME
    return !std::strcmp(_base_type, other._base_type);
#else
    return _base_type == other._base_type;
#endif
}

inline char const* type_info::name() const
{
    return _base_type
#ifndef BP3_TYPE_ID_NAME
        ->name()
#endif
        ;
}


} // namespace bp3

#endif // !BP3_type_id_hpp_INCLUDED
