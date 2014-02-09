#include "bp3/Module.hpp"
#include "bp3/Callable.hpp"

namespace bp3 {

void initExceptions(Module & module) {
    PyPtr base = PyPtr::steal(
        PyErr_NewExceptionWithDoc(
            const_cast<char*>("bp3.WrapperError"),
            const_cast<char*>(
                "Base class for all bp3 exceptions: indicates a failure in mapping C++ to Python"
            ),
            nullptr,
            nullptr
        )
    );
    module.add("WrapperError", base);
}

} // namespace bp3
