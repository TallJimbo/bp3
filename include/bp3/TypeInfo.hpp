#ifndef BP3_type_id_hpp_INCLUDED
#define BP3_type_id_hpp_INCLUDED

#include "bp3/PyPtr.hpp"

#include <typeinfo>
#include <cstring>
#include <string>

namespace bp3 {

// Essentially copied from Boost.Python v2 implementation
// (but with fewer workarounds for old compilers that we
// know we can't support anymore for other reasons).
// This provides a copyable TypeInfo class that works
// across shared library boundaries.

#if (defined(__GNUC__) && __GNUC__ >= 3)       \
 || defined(_AIX) \
 || (   defined(__sgi) && defined(__host_mips)) \
 || (defined(__hpux) && defined(__HP_aCC)) \
 || (defined(linux) && defined(__INTEL_COMPILER) && defined(__ICC))
#define BP3_TYPE_ID_NAME
#endif

struct TypeInfo {

    inline TypeInfo(std::type_info const & = typeid(void));

    inline bool operator==(TypeInfo const & other) const;

    char const * name() const;

    std::string demangle() const;

 private:
#ifdef BP3_TYPE_ID_NAME
    typedef char const* BaseID;
#else
    typedef std::type_info const* BaseID;
#endif
    BaseID _base_type;
};

inline TypeInfo::TypeInfo(std::type_info const & t)
    : _base_type(
#ifdef BP3_TYPE_ID_NAME
        t.name()
#else
        &t
#endif
    ) {}

template <typename T>
inline TypeInfo makeTypeInfo() { return TypeInfo(typeid(T)); }

inline bool TypeInfo::operator==(TypeInfo const & other) const {
#ifdef BP3_TYPE_ID_NAME
    return !std::strcmp(_base_type, other._base_type);
#else
    return _base_type == other._base_type;
#endif
}

inline char const* TypeInfo::name() const
{
    return _base_type
#ifndef BP3_TYPE_ID_NAME
        ->name()
#endif
        ;
}


} // namespace bp3

#endif // !BP3_type_id_hpp_INCLUDED
