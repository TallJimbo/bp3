#ifndef BP3_Overload_hpp_INCLUDED
#define BP3_Overload_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/callables/ArgsFromPython.hpp"
#include "bp3/Module.hpp"

#include <vector>
#include <string>
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
        std::string const & func_name,
        PyPtr const & pyargs, PyPtr const & pykwds,
        OverloadData & data, bool throw_on_failure
    ) const;

    virtual void convertArgs(Module const & mod, OverloadData & data) const = 0;

    virtual void call(OverloadData & data) const = 0;

    virtual ~OverloadBase() {}

protected:

    OverloadBase(std::size_t n_args, std::vector<std::string> kwd_names);

    std::vector<std::string> _kwd_names;
};

typedef std::unique_ptr<OverloadBase> OverloadPtr;

template <typename Result, typename ...Args>
struct Signature {

    static std::size_t const n_args = sizeof...(Args);
    typedef std::function<Result(Args...)> Function;
    typedef ArgsFromPython<Args...> ArgConverter;
};

template <typename Sig, typename Defaults=void>
class Overload : public OverloadBase {
public:

    typedef typename Sig::Function Function;
    typedef typename Sig::ArgConverter ArgConverter;

    virtual void convertArgs(Module const & mod, OverloadData & data) const {
        data.converted_args.reset(
            new ArgConverter(mod, data.unpacked_args)
        );
    }

    Overload(Function func, std::vector<std::string> kwd_names) :
        OverloadBase(Sig::n_args, std::move(kwd_names)), _func(std::move(func))
    {}

private:
    Function _func;
};

template <typename Result, typename ...Args>
inline OverloadPtr makeOverload(Result func(Args...), std::vector<std::string> kwd_names) {
    typedef Signature<Result,Args...> Sig;
    OverloadPtr p(new Overload<Sig>(func, std::move(kwd_names)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline OverloadPtr makeOverload(Result (Class::*func)(Args...), std::vector<std::string> kwd_names) {
    typedef Signature<Result, Class &, Args...> Sig;
    OverloadPtr p(new Overload<Sig>(func, std::move(kwd_names)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline OverloadPtr makeOverload(Result (Class::*func)(Args...) const, std::vector<std::string> kwd_names) {
    typedef Signature<Result, Class const &, Args...> Sig;
    OverloadPtr p(new Overload<Sig>(func, std::move(kwd_names)));
    return p;
}

template <typename Result, typename ...Args>
inline OverloadPtr makeOverload(std::function<Result(Args...)> func, std::vector<std::string> kwd_names) {
    typedef Signature<Result, Args...> Sig;
    OverloadPtr p(new Overload<Sig>(std::move(func), std::move(kwd_names)));
    return p;
}

} // namespace callables
} // namespace bp3

#endif // !BP3_Overload_hpp_INCLUDED
