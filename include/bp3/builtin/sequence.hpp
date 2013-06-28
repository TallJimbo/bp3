#ifndef BP3_BUILTIN_sequence_hpp_INCLUDED
#define BP3_BUILTIN_sequence_hpp_INCLUDED

#include "bp3/builtin/object.hpp"

namespace bp3 { namespace builtin {

class tuple : public detail::builtin_object_base<&PyTuple_Type> {
    typedef detail::builtin_object_base<&PyTuple_Type> base;
public:

    BP3_BUILTIN_CTOR(tuple) // using base::base;

    tuple();

    explicit tuple(object const & other);

    object operator[](Py_ssize_t n) const;

};

class list : public detail::builtin_object_base<&PyList_Type> {
    typedef detail::builtin_object_base<&PyList_Type> base;
public:

    BP3_BUILTIN_CTOR(list) // using base::base;

    list();

    explicit list(object const & other);

    object operator[](Py_ssize_t n) const;

};

}} // namespace bp3::builtin

#endif // !BP3_BUILTIN_sequence_hpp_INCLUDED
