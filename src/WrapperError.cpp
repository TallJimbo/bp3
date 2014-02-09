#include "bp3/WrapperError.hpp"

namespace bp3 {

namespace {

::PyTypeObject const * getTypeFromModule(char const * name) {
    builtin::object bp3mod(PyPtr::steal(PyImport_ImportModule("bp3")));
    builtin::object r1 = getattr(bp3mod, name);
    PyPtr r2 = r1.ptr();
    return reinterpret_cast<PyTypeObject const*>(r2.release());
}

} // anonymous

void WrapperError::raise(std::string const & what) {
    PyErr_SetString(typeobject().ptr().get(), what.c_str());
    builtin::detail::exception_access::fetch_and_throw<WrapperError>();
}

builtin::type WrapperError::typeobject() {
    static ::PyTypeObject const * t = getTypeFromModule("WrapperError");
    // We don't want a destructor to run after Python has shut down, so we have an extra refcount here
    // for the static raw pointer.
    return type(PyPtr::borrow(t));
}

} // namespace bp3
