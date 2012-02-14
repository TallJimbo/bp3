#ifndef BP3_BUILTIN_object_hpp_INCLUDED
#define BP3_BUILTIN_object_hpp_INCLUDED

#include "bp3/py_ptr.hpp"

#include <string>

#define BP3_OBJECT_STANDARD_API(cls, base)                              \
    explicit cls(py_ptr const & ptr) : base(ptr) { require_isinstance(*this, cls::type()); } \
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

class bytes : public object {
public:

    bytes();

    explicit bytes(object const & obj);

    explicit bytes(char const * s);

    explicit bytes(std::string const & s);
    
    BP3_OBJECT_STANDARD_API(bytes, object)

    operator std::string () const;

    char const * c_str() const;

    static object type() {
        return object(py_ptr::borrow(reinterpret_cast<py_ptr::element_type*>(&PyBytes_Type)));
    }
};

class unicode : public object {
public:

    BP3_OBJECT_STANDARD_API(unicode, object)

    explicit unicode(object const & obj);

    explicit unicode(char const * s);

    explicit unicode(std::string const & s);

    static object type() {
        return object(py_ptr::borrow(reinterpret_cast<py_ptr::element_type*>(&PyUnicode_Type)));
    }

};

#if PY_MAJOR_VERSION == 2
typedef bytes str;
#else
typedef unicode str;
#endif

str repr(object const & obj);

}} // namespace bp3::builtin

#endif // !BP3_BUILTIN_object_hpp_INCLUDED
