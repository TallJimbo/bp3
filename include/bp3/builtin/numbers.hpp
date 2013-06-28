#ifndef BP3_BUILTIN_numeric_hpp_INCLUDED
#define BP3_BUILTIN_numeric_hpp_INCLUDED

#include "bp3/py_ptr.hpp"
#include "bp3/builtin/object.hpp"

namespace bp3 { namespace builtin {

class float_ : public detail::builtin_object_base<&PyFloat_Type> {
    typedef detail::builtin_object_base<&PyFloat_Type> base;
public:

    BP3_BUILTIN_CTOR(float_) // using base::base;

    float_(object const & obj);

    explicit float_(double v);

    operator double () const;

};

#if PY_MAJOR_VERSION == 2

class int_ : public detail::builtin_object_base<&PyInt_Type> {
    typedef detail::builtin_object_base<&PyInt_Type> base;
public:

    BP3_BUILTIN_CTOR(int_) // using base::base;

    int_(object const & obj);

    explicit int_(long v);

    operator long () const;

};

class long_ : public detail::builtin_object_base<&PyLong_Type> {
    typedef detail::builtin_object_base<&PyLong_Type> base;
public:

    BP3_BUILTIN_CTOR(long_) // using base::base;

    long_(object const & obj);

    explicit long_(long long v);

    explicit long_(unsigned long long v);

    operator long long () const;

    operator unsigned long long () const;

};

#else

class int_ : public detail::builtin_object_base<&PyLong_Type> {
    typedef detail::builtin_object_base<&PyLong_Type> base;
public:

    BP3_BUILTIN_CTOR(int_) // using base::base;

    int_(object const & obj);

    explicit int_(long long v);

    explicit int_(unsigned long long v);

    operator long long () const;

    operator unsigned long long () const;

};

#endif



}} // namespace bp3::builtin

#endif // !BP3_BUILTIN_numeric_hpp_INCLUDED
