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

class OverloadResolutionError : public WrapperError {
public:

    OverloadResolutionError(OverloadResolutionError const & other) : WrapperError(other) {}

    static builtin::type typeobject();

    static void raise(std::string const & what);

protected:

    friend class detail::ExceptionAccess;

    OverloadResolutionError(PyPtr const & value, PyPtr const & traceback) : WrapperError(value, traceback) {}

};

} // namespace bp3

#endif // !BP3_WrapperError_hpp_INCLUDED
