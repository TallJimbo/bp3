#ifndef BP3_BUILTIN_str_hpp_INCLUDED
#define BP3_BUILTIN_str_hpp_INCLUDED

#include "bp3/py_ptr.hpp"
#include "bp3/builtin/object.hpp"

#include <string>

namespace bp3 { namespace builtin {

class bytes : public detail::builtin_object_base<&PyBytes_Type> {
    typedef detail::builtin_object_base<&PyBytes_Type> base;
public:

    BP3_BUILTIN_CTOR(bytes) // using base::base;

    bytes();

    explicit bytes(object const & obj);

    explicit bytes(char const * s);

    explicit bytes(std::string const & s);

    operator std::string () const;

    char const * c_str() const;

};

class unicode : public detail::builtin_object_base<&PyUnicode_Type> {
    typedef detail::builtin_object_base<&PyUnicode_Type> base;
public:

    BP3_BUILTIN_CTOR(unicode) // using base::base;

    // empty string
    unicode();

    // equivalent to unicode(obj) in Python 2, or str(obj) in Python 3.
    explicit unicode(object const & obj);

    // UTF-8
    explicit unicode(char const * s);
    explicit unicode(std::string const & s);

    operator std::string () const;

};

#if PY_MAJOR_VERSION == 2
typedef bytes str;
#else
typedef unicode str;
#endif

str repr(object const & obj);

}} // namespace bp3::builtin

#endif // !BP3_BUILTIN_str_hpp_INCLUDED
