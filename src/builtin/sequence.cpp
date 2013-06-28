#include "bp3/builtin/sequence.hpp"
#include "bp3/builtin/exceptions.hpp"

namespace bp3 { namespace builtin {

tuple::tuple() : base(py_ptr::steal(PyTuple_New(0))) {}

tuple::tuple(object const & other) : base(py_ptr::steal(PySequence_Tuple(other.ptr().get()))) {}

object tuple::operator[](Py_ssize_t n) const {
        return object(py_ptr::borrow(PyTuple_GetItem(ptr().get(), n)));
}

list::list() : base(py_ptr::steal(PyList_New(0))) {}

list::list(object const & other) : base(py_ptr::steal(PySequence_List(other.ptr().get()))) {}

object list::operator[](Py_ssize_t n) const {
    return object(py_ptr::borrow(PyList_GetItem(ptr().get(), n)));
}

}} // namespace bp3::builtin
