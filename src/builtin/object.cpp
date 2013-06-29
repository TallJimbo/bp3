#include "bp3/builtin/object.hpp"
#include "bp3/builtin/exceptions.hpp"

namespace bp3 {

PyPtr const & PyPtr::raise_if_null() const {
    if (!_p) {
        if (PyErr_Occurred()) {
            throw_error_already_set();
        } else {
            builtin::SystemError::raise("Unexpected null PyObject* with no exception set.");
        }
    }
    return *this;
}

PyPtr const & PyPtr::raise_if_not_isinstance(PyPtr const & cls) const {
    static std::string message = " is not an instance of ";
    raise_if_null();
    int r = PyObject_IsInstance(this->get(), cls.get());
    if (r < 0) throw_error_already_set();
    if (r == 0) {
        PyPtr ps1 = PyPtr::steal(PyObject_Repr(this->get())); ps1.raise_if_null();
        PyPtr ps2 = PyPtr::steal(PyObject_Repr(cls.get())); ps2.raise_if_null();
        // FIXME: probably won't work with Python 3.x
        char const * s1 = PyBytes_AsString(ps1.get()); if (!s1) throw_error_already_set();
        char const * s2 = PyBytes_AsString(ps2.get()); if (!s2) throw_error_already_set();
        builtin::TypeError::raise(s1 + message + s2);
    }
	return *this;
}

namespace builtin {

type::type(object const & obj) : object(PyPtr::steal(PyObject_Type(obj.ptr().get()))) {}

type::type(PyPtr const & ptr) :
    object(ptr.raise_if_not_isinstance(PyPtr::borrow(reinterpret_cast<PyObject*>(&PyType_Type))))
{}

type::type(object const & name, object const & bases, object const & dict) :
    object(
        PyPtr::steal(
            PyObject_CallFunctionObjArgs(
                reinterpret_cast<PyObject*>(&PyType_Type),
                name.ptr().get(), bases.ptr().get(), dict.ptr().get(), nullptr
            )
        )
    )
{}

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

object getattr(object const & inst, char const * name) {
    return object(PyPtr::steal(PyObject_GetAttrString(inst.ptr().get(), name)));
}

object getattr(object const & inst, object const & name) {
    return object(PyPtr::steal(PyObject_GetAttr(inst.ptr().get(), name.ptr().get())));
}

void setattr(object const & inst, char const * name, object const & value) {
    if (PyObject_SetAttrString(inst.ptr().get(), name, value.ptr().get()) < 0) {
        throw_error_already_set();
    }
}

void setattr(object const & inst, object const & name, object const & value) {
    if (PyObject_SetAttr(inst.ptr().get(), name.ptr().get(), value.ptr().get()) < 0) {
        throw_error_already_set();
    }
}

void delattr(object const & inst, char const * name) {
    if (PyObject_DelAttrString(inst.ptr().get(), name) < 0) {
        throw_error_already_set();
    }
}

void delattr(object const & inst, object const & name) {
    if (PyObject_DelAttr(inst.ptr().get(), name.ptr().get()) < 0) {
        throw_error_already_set();
    }
}

}} // namespace bp3::builtin
