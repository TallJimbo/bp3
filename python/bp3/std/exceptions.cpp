#include "bp3/Module.hpp"
#include "bp3/Callable.hpp"

namespace bp3 { namespace std {

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
        module, "bp3.std.exception", "exception",
        "Python wrapper for std::exception: the C++ standard library's exception base class",
        nullptr
    );
    makeException(
        module, "bp3.std.bad_alloc", "bad_alloc",
        "Python wrapper for std::bad_alloc: thrown on failure to allocate memory in C++",
        builtin::tuple({base, builtin::MemoryError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.std.bad_cast", "bad_cast",
        "Python wrapper for std::bad_cast: thrown on a failure to dynamic_cast a C++ object",
        builtin::tuple({base, builtin::TypeError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.std.bad_weak_ptr", "bad_weak_ptr",
        "Python wrapper for std::bad_weak_ptr: thrown when a shared_ptr is constructed with a null weak_ptr",
        builtin::tuple({base, builtin::ReferenceError::typeobject()}).ptr().release()
    );
    builtin::type logic = makeException(
        module, "bp3.std.logic_error", "logic_error",
        "Python wrapper for std::logic_error: logic exception in C++",
        builtin::tuple({base}).ptr().release()
    );
    makeException(
        module, "bp3.std.domain_error", "domain_error",
        "Python wrapper for std::domain_error: domain error in C++",
        builtin::tuple({logic}).ptr().release()
    );
    makeException(
        module, "bp3.std.invalid_argument", "invalid_argument",
        "Python wrapper for std::invalid_argument: invalid argument to a C++ function",
        builtin::tuple({logic, builtin::ValueError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.std.length_error", "length_error",
        "Python wrapper for std::length_error: length error in C++",
        builtin::tuple({logic}).ptr().release()
    );
    makeException(
        module, "bp3.std.out_of_range", "out_of_range",
        "Python wrapper for std::out_of_range: C++ exception indicating an out-of-range index",
        builtin::tuple({logic, builtin::IndexError::typeobject()}).ptr().release()
    );
    builtin::type runtime = makeException(
        module, "bp3.std.runtime_error", "runtime_error",
        "Python wrapper for std::runtime_error: runtime exception in C++",
        builtin::tuple({base, builtin::RuntimeError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.std.overflow_error", "overflow_error",
        "Python wrapper for std::overflow_error: C++ exception indicating numeric overflow",
        builtin::tuple({runtime, builtin::OverflowError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.std.underflow_error", "underflow_error",
        "Python wrapper for std::underflow_error: C++ exception indicating numeric underflow",
        builtin::tuple({runtime, builtin::ArithmeticError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.std.range_error", "range_error",
        "Python wrapper for std::range_error: range error in C++",
        builtin::tuple({runtime, builtin::ArithmeticError::typeobject()}).ptr().release()
    );
    builtin::type system = makeException(
        module, "bp3.std.system_error", "system_error",
        "Python wrapper for std::system_error: operating system or environment error in C++",
        builtin::tuple({runtime, builtin::EnvironmentError::typeobject()}).ptr().release()
    );
    makeException(
        module, "bp3.std.ios_base_failure", "ios_base_failure",
        "Python wrapper for std::ios_base::failure: stream error in C++",
        builtin::tuple({system, builtin::IOError::typeobject()}).ptr().release()
    );
}

}} // namespace bp3::std
