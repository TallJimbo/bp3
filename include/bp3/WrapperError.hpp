#ifndef BP3_WrapperError_hpp_INCLUDED
#define BP3_WrapperError_hpp_INCLUDED

#include "bp3/builtin/exceptions.hpp"

namespace bp3 {

class WrapperError : public builtin::Exception {
public:

    WrapperError(WrapperError const & other) : builtin::Exception(other) {}

    static builtin::type typeobject();

    static void raise(std::string const & what);

protected:

    friend class detail::ExceptionAccess;

    WrapperError(PyPtr const & value, PyPtr const & traceback) : builtin::Exception(value, traceback) {}

};

#define BP3_DECLARE_WRAPPER_ERROR_DERIVED(NAME)                         \
    class NAME : public WrapperError {                                  \
    public:                                                             \
        NAME(NAME const & other) : WrapperError(other) {}               \
        static builtin::type typeobject();                              \
        static void raise(std::string const & what);                    \
    protected:                                                          \
        friend class detail::ExceptionAccess;                           \
        NAME(PyPtr const & value, PyPtr const & traceback) : WrapperError(value, traceback) {} \
    }

BP3_DECLARE_WRAPPER_ERROR_DERIVED(OverloadResolutionError);
BP3_DECLARE_WRAPPER_ERROR_DERIVED(SignatureError);
BP3_DECLARE_WRAPPER_ERROR_DERIVED(FromPythonTypeError);
BP3_DECLARE_WRAPPER_ERROR_DERIVED(UnknownError);

#undef BP3_DECLARE_WRAPPER_ERROR_DERIVED

} // namespace bp3

#endif // !BP3_WrapperError_hpp_INCLUDED
