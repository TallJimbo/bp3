#ifndef BP3_PyPtr_hpp_INCLUDED
#define BP3_PyPtr_hpp_INCLUDED

#include "Python.h"
#include <memory>

namespace bp3 {

class PyPtr {
public:

    typedef ::PyObject element_type;

    PyPtr() : _p(nullptr) {}

    PyPtr(PyPtr const & other) : _p(other._p) { Py_XINCREF(_p); }

    ~PyPtr() { Py_XDECREF(_p); }

    PyPtr & operator=(PyPtr const & other) {
        reset(other._p);
        return *this;
    }

    explicit operator bool() const { return _p; }

    void reset(element_type * p=nullptr) {
        if (p != _p) {
            Py_XDECREF(_p);
            _p = p;
            Py_XINCREF(_p);
        }
    }

    void swap(PyPtr & other) {
        std::swap(_p, other._p);
    }

    element_type * release() {
        element_type * r = _p;
        _p = nullptr;
        return r;
    }

    element_type * incref() const {
        Py_XINCREF(_p);
        return _p;
    }

    element_type * get() const { return _p; }

    element_type * operator->() const { return _p; }

    element_type & operator*() const { return *_p; }

    PyPtr const & raise_if_null() const;

    PyPtr const & raise_if_not_isinstance(PyPtr const & cls) const;

    static inline PyPtr steal(element_type * p) {
        return PyPtr(p);
    }

    static inline PyPtr borrow(element_type * p) {
        Py_XINCREF(p);
        return PyPtr(p);
    }

private:

    explicit PyPtr(element_type * p) : _p(p) {}

    element_type * _p;
};

inline void swap(PyPtr & a, PyPtr & b) { a.swap(b); }

inline bool operator==(PyPtr const & a, PyPtr const & b) { return a.get() == b.get(); }
inline bool operator!=(PyPtr const & a, PyPtr const & b) { return a.get() != b.get(); }

} // namespace bp3

#endif // !BP3_PyPtr_hpp_INCLUDED
