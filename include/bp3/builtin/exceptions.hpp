#ifndef BP3_BUILTIN_exceptions_hpp_INCLUDED
#define BP3_BUILTIN_exceptions_hpp_INCLUDED

#include "bp3/builtin/object.hpp"

#define BP3_BUILTIN_EXCEPTION(cls, base)                                \
    class cls : public base {                                           \
    public:                                                             \
    cls(cls const & other) : base(other) {}                             \
    static object type() { return object(py_ptr::borrow(PyExc_ ##cls)); } \
    static void raise(std::string const & what) { raise_impl(what, type()); } \
    protected:                                                            \
    friend class exceptions::access;                                    \
    cls(py_ptr const & value, py_ptr const & traceback) : base(value, traceback) {} \
    }

namespace bp3 {

void throw_error_already_set();

namespace builtin {

namespace exceptions {

class access;

} // namespace exceptions

class Exception : public object {
public:

    static void raise(std::string const & what) { raise_impl(what, type()); }

    static object type() { return object(py_ptr::borrow(PyExc_Exception)); }

protected:

    friend class exceptions::access;

    static void raise_impl(std::string const & what, object const & type);

    Exception(py_ptr const & value, py_ptr const & traceback) : object(value), _traceback(traceback) {}

    py_ptr _traceback;
};

BP3_BUILTIN_EXCEPTION(StandardError, Exception);
BP3_BUILTIN_EXCEPTION(SystemError, StandardError);
BP3_BUILTIN_EXCEPTION(TypeError, StandardError);

}} // namespace bp3::builtin

#undef BP3_BUILTIN_EXCEPTION

#endif // !BP3_BUILTIN_exceptions_hpp_INCLUDED
