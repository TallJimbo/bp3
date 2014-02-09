#include "bp3/WrapperError.hpp"
#include "bp3/ExceptionAccess.hpp"

namespace bp3 {

namespace {

::PyObject * getTypeFromModule(char const * name) {
    builtin::object bp3mod(PyPtr::steal(PyImport_ImportModule("bp3")));
    builtin::object r1 = getattr(bp3mod, name);
    PyPtr r2 = r1.ptr();
    return r2.release();
}

} // anonymous

void WrapperError::raise(std::string const & what) {
    PyErr_SetString(typeobject().ptr().get(), what.c_str());
    detail::ExceptionAccess::fetch_and_throw<WrapperError>();
}

builtin::type WrapperError::typeobject() {
    static ::PyObject * t = getTypeFromModule("WrapperError");
    // We don't want a destructor to run after Python has shut down, so we have an extra refcount here
    // for the static raw pointer.
    return builtin::type(PyPtr::borrow(t));
}

} // namespace bp3
