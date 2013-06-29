#ifndef BP3_Overload_hpp_INCLUDED
#define BP3_Overload_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/callables/ArgDef.hpp"
#include "bp3/callables/ArgsFromPython.hpp"
#include "bp3/Module.hpp"

#include <functional>

namespace bp3 {
namespace callables {

struct OverloadData {
    bool unpack_successful;
    std::vector<PyPtr> unpacked_args;
    std::unique_ptr<ArgsFromPythonBase> converted_args;
};

class OverloadBase {
public:

    OverloadBase() = delete;

    void unpackArgs(
        std::string const & name, PyPtr const & args, PyPtr const & kwds, OverloadData & data,
        bool throw_on_failure
    ) const;

    virtual void convertArgs(Module const & mod, OverloadData & data) const = 0;

    virtual void call(OverloadData & data) const = 0;

    virtual ~OverloadBase() {}

protected:

    explicit OverloadBase(std::size_t n_args, ArgDefList const & arg_defs);

    ArgDefList _arg_defs;
};

typedef std::unique_ptr<OverloadBase> OverloadPtr;

template <typename Result, typename ...Args>
class Overload : public OverloadBase {
public:

    virtual void convertArgs(Module const & mod, OverloadData & data) const {
        data.converted_args.reset(
            new ArgsFromPython<Args...>(mod, data.unpacked_args)
        );
    }

    Overload(std::function<Result(Args...)> func, ArgDefList arg_defs) :
        OverloadBase(sizeof...(Args), std::move(arg_defs)), _func(std::move(func))
    {}

private:
    std::function<Result(Args...)> _func;
};

template <typename Result, typename ...Args>
inline OverloadPtr makeOverload(Result func(Args...), ArgDefList arg_defs) {
    OverloadPtr p(new Overload<Result,Args...>(func, std::move(arg_defs)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline OverloadPtr makeOverload(Result (Class::*func)(Args...), ArgDefList arg_defs) {
    OverloadPtr p(new Overload<Result, Class &, Args...>(func, std::move(arg_defs)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline OverloadPtr makeOverload(Result (Class::*func)(Args...) const, ArgDefList arg_defs) {
    OverloadPtr p(new Overload<Result, Class const &, Args...>(func, std::move(arg_defs)));
    return p;
}

template <typename Result, typename ...Args>
inline OverloadPtr makeOverload(std::function<Result(Args...)> func, ArgDefList arg_defs) {
    OverloadPtr p(new Overload<Result, Args...>(std::move(func), std::move(arg_defs)));
    return p;
}

} // namespace callables
} // namespace bp3

#endif // !BP3_Overload_hpp_INCLUDED
