#ifndef BP3_BUILTIN_exceptions_hpp_INCLUDED
#define BP3_BUILTIN_exceptions_hpp_INCLUDED

#include "bp3/builtin/object.hpp"

#define BP3_BUILTIN_EXCEPTION(cls, base)                                \
    class cls : public base {                                           \
    public:                                                             \
        cls(cls const & other) : base(other) {}                             \
        static type typeobject() { return type(py_ptr::borrow(PyExc_ ##cls)); } \
        static void raise(std::string const & what);                    \
    protected:                                                          \
        friend class detail::exception_access;                              \
        cls(py_ptr const & value, py_ptr const & traceback) : base(value, traceback) {} \
    }

namespace bp3 {

void throw_error_already_set();

namespace builtin {

namespace detail {

class exception_access;

} // namespace detail

class Exception {
public:

    Exception & operator=(Exception const &) = delete;

    py_ptr const & ptr() const { return _value.ptr(); }

    operator object const & () const { return _value; }

    std::nullptr_t release();

    static void raise(std::string const & msg);

    static object typeobject() { return type(py_ptr::borrow(PyExc_Exception)); }

protected:

    friend class detail::exception_access;

    Exception(py_ptr const & value, py_ptr const & traceback) : _value(value), _traceback(traceback) {}

    object _value;
    py_ptr _traceback;
};

#if PY_MAJOR_VERSION == 2
BP3_BUILTIN_EXCEPTION(StandardError, Exception);
#else 
#define StandardError Exception
#endif
BP3_BUILTIN_EXCEPTION(SystemError, StandardError);
BP3_BUILTIN_EXCEPTION(TypeError, StandardError);

}} // namespace bp3::builtin

#undef BP3_BUILTIN_EXCEPTION
#if PY_MAJOR_VERSION != 2
#undef StandardError
#endif

#endif // !BP3_BUILTIN_exceptions_hpp_INCLUDED
