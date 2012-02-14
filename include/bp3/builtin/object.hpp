#ifndef BP3_BUILTIN_object_hpp_INCLUDED
#define BP3_BUILTIN_object_hpp_INCLUDED

#include "bp3/py_ptr.hpp"

#include <string>

#define BP3_OBJECT_STANDARD_API(cls, base)                              \
    explicit cls(py_ptr const & ptr) : base(ptr) { require_isinstance(*this, cls::type()); } \
    explicit cls(object const & other) : base(other) { require_isinstance(*this, cls::type()); } \
    cls(cls const & other) : base(other) {}

namespace bp3 { namespace builtin {

class object {
public:

    object() : _ptr(py_ptr::steal(Py_None)) {}

    explicit object(py_ptr const & ptr) : _ptr(ptr) {}

    object & operator=(object const & other) {
        _ptr = other._ptr;
        return *this;
    }

    py_ptr const & ptr() const { return _ptr; }

protected:
    py_ptr _ptr;
};

Py_ssize_t len(object const & obj);

bool isinstance(object const & inst, object const & cls);

void require_isinstance(object const & inst, object const & cls);

class tuple : public object {
public:

    tuple() : object(py_ptr::steal(PyTuple_New(0))) {}

    BP3_OBJECT_STANDARD_API(tuple, object)

    object operator[](Py_ssize_t n) const {
        return object(py_ptr::borrow(PyTuple_GetItem(_ptr.get(), n)));
    }

    static object type() {
        return object(py_ptr::borrow(reinterpret_cast<py_ptr::element_type*>(&PyTuple_Type)));
    }
};

class str : public object {
public:

    str() : object(py_ptr::steal(PyBytes_FromString(""))) {}

    BP3_OBJECT_STANDARD_API(str, object)

    operator std::string () const { return PyBytes_AS_STRING(_ptr.get()); }

    char const * c_str() const { return PyBytes_AS_STRING(_ptr.get()); }

    static object type() {
        return object(py_ptr::borrow(reinterpret_cast<py_ptr::element_type*>(&PyBytes_Type)));
    }
};

typedef str bytes;

str repr(object const & obj);

}} // namespace bp3::builtin

#endif // !BP3_BUILTIN_object_hpp_INCLUDED
