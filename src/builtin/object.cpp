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

str repr(object const & obj) {
    return str(py_ptr::steal(PyObject_Repr(obj.ptr().get())).raise_if_null());
}

void require_isinstance(object const & inst, object const & cls) {
    static std::string message = " is not an instance of ";
    if (!isinstance(inst, cls)) {
        py_ptr ps1 = py_ptr::steal(PyObject_Repr(inst.ptr().get())).raise_if_null();
        py_ptr ps2 = py_ptr::steal(PyObject_Repr(cls.ptr().get())).raise_if_null();
        char const * s1 = PyString_AsString(ps1.get()); if (!s1) throw_error_already_set();
        char const * s2 = PyString_AsString(ps2.get()); if (!s2) throw_error_already_set();
        TypeError::raise(s1 + message + s2);
    }
}

}} // namespace bp3::builtin
