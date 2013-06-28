#ifndef BP3_BUILTIN_object_hpp_INCLUDED
#define BP3_BUILTIN_object_hpp_INCLUDED

#include "bp3/py_ptr.hpp"

#include <string>

#define BP3_BUILTIN_CTOR(cls) cls(py_ptr const & ptr) : base(ptr) {}

namespace bp3 { namespace builtin {

class object {
public:

    object() : _ptr(py_ptr::steal(Py_None)) {}

    explicit object(py_ptr const & ptr) : _ptr(ptr.raise_if_null()) {}

    py_ptr const & ptr() const { return _ptr; }

protected:
    py_ptr _ptr;
};

class type {
public:

    type(object const & name, object const & bases, object const & dict);

    explicit type(object const & obj);

    type(py_ptr const & ptr);

    py_ptr const & ptr() const { return _obj.ptr(); }

    operator object const & () const { return _obj; }

    static type typeobject() {
        return type(py_ptr::borrow(reinterpret_cast<PyObject*>(&PyType_Type)));
    }

private:
    object _obj;
};

namespace detail {

template <PyTypeObject * Type>
class builtin_object_base {
public:

    explicit builtin_object_base(py_ptr const & ptr) :
        _obj(ptr.raise_if_not_isinstance(py_ptr::borrow(reinterpret_cast<PyObject*>(Type)))) {}

    py_ptr const & ptr() const { return _obj.ptr(); }

    operator object const & () const { return _obj; }

    static type typeobject() { return type(py_ptr::borrow(reinterpret_cast<PyObject*>(Type))); }

private:
    object _obj;
};

} // namespace detail

Py_ssize_t len(object const & obj);

bool isinstance(object const & inst, object const & cls);

}} // namespace bp3::builtin

#endif // !BP3_BUILTIN_object_hpp_INCLUDED
