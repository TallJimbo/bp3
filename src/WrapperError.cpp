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

#define BP3_WRAPPER_ERROR_IMPL(NAME)            \
    void NAME::raise(std::string const & what) {                \
        PyErr_SetString(typeobject().ptr().get(), what.c_str());    \
        detail::ExceptionAccess::fetch_and_throw<NAME>();           \
    }                                                               \
    builtin::type NAME::typeobject() {                              \
        static ::PyObject * t = getTypeFromModule(#NAME);  \
        return builtin::type(PyPtr::borrow(t));                     \
    }

BP3_WRAPPER_ERROR_IMPL(WrapperError)
BP3_WRAPPER_ERROR_IMPL(OverloadResolutionError)
BP3_WRAPPER_ERROR_IMPL(SignatureError)
BP3_WRAPPER_ERROR_IMPL(FromPythonTypeError)
BP3_WRAPPER_ERROR_IMPL(UnknownError)

#undef BP3_WRAPPER_ERROR_IMPL

} // namespace bp3
