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

class BaseException {
public:

    BaseException & operator=(BaseException const &) = delete;

    py_ptr const & ptr() const { return _value.ptr(); }

    operator object const & () const { return _value; }

    std::nullptr_t release();

    static void raise(std::string const & msg);

    static object typeobject() { return type(py_ptr::borrow(PyExc_BaseException)); }

protected:

    friend class detail::exception_access;

    BaseException(py_ptr const & value, py_ptr const & traceback) : _value(value), _traceback(traceback) {}

    object _value;
    py_ptr _traceback;
};

BP3_BUILTIN_EXCEPTION(Exception, BaseException);
BP3_BUILTIN_EXCEPTION(StopIteration, Exception);
#if PY_MAJOR_VERSION == 2
BP3_BUILTIN_EXCEPTION(StandardError, Exception);
#define ERROR_BASE StandardError
#else
#define ERROR_BASE Exception
#endif
BP3_BUILTIN_EXCEPTION(ArithmeticError, ERROR_BASE);
BP3_BUILTIN_EXCEPTION(FloatingPointError, ArithmeticError);
BP3_BUILTIN_EXCEPTION(OverflowError, ArithmeticError);
BP3_BUILTIN_EXCEPTION(ZeroDivisionError, ArithmeticError);
BP3_BUILTIN_EXCEPTION(SystemError, ERROR_BASE);
BP3_BUILTIN_EXCEPTION(TypeError, ERROR_BASE);

}} // namespace bp3::builtin

#undef BP3_BUILTIN_EXCEPTION
#undef ERROR_BASE

#endif // !BP3_BUILTIN_exceptions_hpp_INCLUDED
