#ifndef BP3_py_ptr_hpp_INCLUDED
#define BP3_py_ptr_hpp_INCLUDED

#include "Python.h"
#include <memory>

namespace bp3 {

class py_ptr {
public:

    typedef ::PyObject element_type;

    py_ptr() : _p(nullptr) {}

    py_ptr(py_ptr const & other) : _p(other._p) { Py_XINCREF(_p); }

    ~py_ptr() { Py_XDECREF(_p); }

    py_ptr & operator=(py_ptr const & other) {
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

    void swap(py_ptr & other) {
        std::swap(_p, other._p);
    }

    element_type * release() {
        element_type * r = _p;
        _p = nullptr;
        return r;
    }

    element_type * get() const { return _p; }

    element_type * operator->() const { return _p; }

    element_type & operator*() const { return *_p; }

    py_ptr const & raise_if_null() const;

    py_ptr const & raise_if_not_isinstance(py_ptr const & cls) const;

    static inline py_ptr steal(element_type * p) {
        return py_ptr(p);
    }

    static inline py_ptr borrow(element_type * p) {
        Py_XINCREF(p);
        return py_ptr(p);
    }

private:

    explicit py_ptr(element_type * p) : _p(p) {}

    element_type * _p;
};

inline void swap(py_ptr & a, py_ptr & b) { a.swap(b); }

inline bool operator==(py_ptr const & a, py_ptr const & b) { return a.get() == b.get(); }
inline bool operator!=(py_ptr const & a, py_ptr const & b) { return a.get() != b.get(); }

} // namespace bp3

#endif // !BP3_py_ptr_hpp_INCLUDED
