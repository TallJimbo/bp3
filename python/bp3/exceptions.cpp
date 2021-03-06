#include "bp3/Module.hpp"
#include "bp3/Callable.hpp"

namespace bp3 {

namespace {

builtin::type makeException(
    Module & module,
    char const * full_name,
    char const * name,
    char const * doc,
    PyObject * bases
) {
    builtin::type t(
        PyPtr::steal(
            PyErr_NewExceptionWithDoc(
                const_cast<char*>(full_name),
                const_cast<char*>(doc),
                bases,
                nullptr
            )
        )
    );
    module.add(name, t.ptr());
    return t;
}

} // anonymous

void initExceptions(Module & module) {
    builtin::type base = makeException(
        module, "bp3.WrapperError", "WrapperError",
        "Base class for all bp3 exceptions: indicates a failure in mapping C++ to Python",
        nullptr
    );
    makeException(
        module, "bp3.OverloadResolutionError", "OverloadResolutionError",
        "Exception raised when the Python arguments passed to a C++ function match multiple overloads "
        "equally well.",
        builtin::tuple({base, builtin::TypeError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.FromPythonTypeError", "FromPythonTypeError",
        "Exception raised when a Python object cannot be converted to the desired C++ type.",
        builtin::tuple({base, builtin::TypeError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.SignatureError", "SignatureError",
        "Exception raised when a the Python arguments passed to a C++ function do not match its signature.",
        builtin::tuple({base, builtin::TypeError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.UnknownError", "UnknownError",
        "Exception raised an when a C++ object that does not inherit from std::exception is thrown.",
        builtin::tuple({base}).ptr().release()
    );
}

} // namespace bp3
