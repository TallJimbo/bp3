#ifndef BP3_BUILTIN_sequence_hpp_INCLUDED
#define BP3_BUILTIN_sequence_hpp_INCLUDED

#include "bp3/builtin/object.hpp"

namespace bp3 { namespace builtin {

class slice : public detail::builtin_object_base<&PySlice_Type> {
    typedef detail::builtin_object_base<&PySlice_Type> base;
public:

    BP3_BUILTIN_CTOR(slice) // using base::base;

    slice(object const & other);

    slice(Py_ssize_t start, Py_ssize_t stop);

    slice(Py_ssize_t start, Py_ssize_t stop, Py_ssize_t step);

    slice(object const & start, object const & stop, object const & step=object());

};

class tuple : public detail::builtin_object_base<&PyTuple_Type> {
    typedef detail::builtin_object_base<&PyTuple_Type> base;
public:

    BP3_BUILTIN_CTOR(tuple) // using base::base;

    tuple();

    tuple(object const & other);

    tuple(std::initializer_list<PyPtr> x);

    tuple(std::initializer_list<object> x);

    object operator[](Py_ssize_t n) const;

    tuple operator+(tuple const & other) const;

    tuple operator*(Py_ssize_t n) const;

};

class list : public detail::builtin_object_base<&PyList_Type> {
    typedef detail::builtin_object_base<&PyList_Type> base;
public:

    BP3_BUILTIN_CTOR(list) // using base::base;

    list();

    list(object const & other);

    list(std::initializer_list<PyPtr> x);

    list(std::initializer_list<object> x);

    object operator[](Py_ssize_t n) const;

    list operator+(list const & other) const;

    list & operator+=(list const & other);

    list operator*(Py_ssize_t n) const;

    list & operator*=(Py_ssize_t n);

};

}} // namespace bp3::builtin

#endif // !BP3_BUILTIN_sequence_hpp_INCLUDED
