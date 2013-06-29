#ifndef BP3_BUILTIN_object_hpp_INCLUDED
#define BP3_BUILTIN_object_hpp_INCLUDED

#include "bp3/PyPtr.hpp"

#include <string>

#define BP3_BUILTIN_CTOR(cls) cls(PyPtr const & ptr) : base(ptr) {}

namespace bp3 { namespace builtin {

class object {
public:

    object() : _ptr(PyPtr::steal(Py_None)) {}

    explicit object(PyPtr const & ptr) : _ptr(ptr.raise_if_null()) {}

    PyPtr const & ptr() const { return _ptr; }

protected:
    PyPtr _ptr;
};

class type : public object {
public:

    type(object const & name, object const & bases, object const & dict);

    type(object const & obj);

    explicit type(PyPtr const & ptr);

    static type typeobject() {
        return type(PyPtr::borrow(reinterpret_cast<PyObject*>(&PyType_Type)));
    }

};

namespace detail {

template <PyTypeObject * Type>
class builtin_object_base : public object {
public:

    explicit builtin_object_base(PyPtr const & ptr) :
        object(ptr.raise_if_not_isinstance(PyPtr::borrow(reinterpret_cast<PyObject*>(Type)))) {}

    static type typeobject() { return type(PyPtr::borrow(reinterpret_cast<PyObject*>(Type))); }

};

} // namespace detail

Py_ssize_t len(object const & obj);

bool isinstance(object const & inst, object const & cls);

object getattr(object const & inst, char const * name);

object getattr(object const & inst, object const & name);

void setattr(object const & inst, char const * name, object const & value);

void setattr(object const & inst, object const & name, object const & value);

void delattr(object const & inst, char const * name);

void delattr(object const & inst, object const & name);

}} // namespace bp3::builtin

#endif // !BP3_BUILTIN_object_hpp_INCLUDED
