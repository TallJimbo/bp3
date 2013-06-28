#include "bp3/builtin/sequence.hpp"
#include "bp3/builtin/exceptions.hpp"

namespace bp3 { namespace builtin {

slice::slice(Py_ssize_t start, Py_ssize_t stop) :
    base(
        py_ptr::steal(
            PySlice_New(
                py_ptr::steal(PyLong_FromLong(start)).release(),
                py_ptr::steal(PyLong_FromLong(stop)).release(),
                nullptr
            )
        )
    )
{}

slice::slice(Py_ssize_t start, Py_ssize_t stop, Py_ssize_t step) :
    base(
        py_ptr::steal(
            PySlice_New(
                py_ptr::steal(PyLong_FromLong(start)).release(),
                py_ptr::steal(PyLong_FromLong(stop)).release(),
                py_ptr::steal(PyLong_FromLong(step)).release()
            )
        )
    )
{}

slice::slice(object const & start, object const & stop, object const & step) :
    base(py_ptr::steal(PySlice_New(start.ptr().get(), stop.ptr().get(), step.ptr().get())))
{}

tuple::tuple() : base(py_ptr::steal(PyTuple_New(0))) {}

tuple::tuple(object const & other) : base(py_ptr::steal(PySequence_Tuple(other.ptr().get()))) {}

object tuple::operator[](Py_ssize_t n) const {
    return object(py_ptr::borrow(PyTuple_GetItem(ptr().get(), n)));
}

tuple tuple::operator+(tuple const & other) const {
    return tuple(py_ptr::steal(PySequence_Concat(ptr().get(), other.ptr().get())));
}

tuple tuple::operator*(Py_ssize_t n) const {
    return tuple(py_ptr::steal(PySequence_Repeat(ptr().get(), n)));
}


list::list() : base(py_ptr::steal(PyList_New(0))) {}

list::list(object const & other) : base(py_ptr::steal(PySequence_List(other.ptr().get()))) {}

object list::operator[](Py_ssize_t n) const {
    return object(py_ptr::borrow(PyList_GetItem(ptr().get(), n)));
}

list list::operator+(list const & other) const {
    return list(py_ptr::steal(PySequence_Concat(ptr().get(), other.ptr().get())));
}

list & list::operator+=(list const & other) {
    py_ptr::steal(PySequence_InPlaceConcat(ptr().get(), other.ptr().get())).raise_if_null();
    return *this;
}

list list::operator*(Py_ssize_t n) const {
    return list(py_ptr::steal(PySequence_Repeat(ptr().get(), n)));
}

list & list::operator*=(Py_ssize_t n) {
    py_ptr::steal(PySequence_InPlaceRepeat(ptr().get(), n)).raise_if_null();
    return *this;
}

}} // namespace bp3::builtin
