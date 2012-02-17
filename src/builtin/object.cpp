#include "bp3/builtin/object.hpp"
#include "bp3/builtin/exceptions.hpp"

namespace bp3 { 

py_ptr const & py_ptr::raise_if_null() const {
    if (!_p) {
        if (PyErr_Occurred()) {
            throw_error_already_set();
        } else {
            builtin::SystemError::raise("Unexpected null PyObject* with no exception set.");
        }
    }
    return *this;
}

py_ptr const & py_ptr::raise_if_not_isinstance(py_ptr const & cls) const {
    static std::string message = " is not an instance of ";
    raise_if_null();
    int r = PyObject_IsInstance(this->get(), cls.get());
    if (r < 0) throw_error_already_set();
    if (r == 0) {
        py_ptr ps1 = py_ptr::steal(PyObject_Repr(this->get())).raise_if_null();
        py_ptr ps2 = py_ptr::steal(PyObject_Repr(cls.get())).raise_if_null();
        // FIXME: probably won't work with Python 3.x
        char const * s1 = PyBytes_AsString(ps1.get()); if (!s1) throw_error_already_set();
        char const * s2 = PyBytes_AsString(ps2.get()); if (!s2) throw_error_already_set();
        builtin::TypeError::raise(s1 + message + s2);
    }
}

namespace builtin {

Py_ssize_t len(object const & obj) {
    Py_ssize_t r = PyObject_Size(obj.ptr().get());
    if (r < 0) throw_error_already_set();
    return r;
}

bool isinstance(object const & inst, object const & cls) {
    int r = PyObject_IsInstance(inst.ptr().get(), cls.ptr().get());
    if (r < 0) throw_error_already_set();
    return r;
}

type::type(object const & name, object const & bases, object const & dict) :
    _obj(
        py_ptr::steal(
            PyObject_CallFunctionObjArgs(
                reinterpret_cast<PyObject*>(&PyType_Type),
                name.ptr().get(), bases.ptr().get(), dict.ptr().get(), nullptr
            )
        )
    )
{}

}} // namespace bp3::builtin
