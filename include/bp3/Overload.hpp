#ifndef BP3_Overload_hpp_INCLUDED
#define BP3_Overload_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/ArgsFromPython.hpp"
#include "bp3/Registry.hpp"

#include <vector>
#include <string>
#include <functional>

namespace bp3 {

struct OverloadResolutionData;

class OverloadBase {
public:

    OverloadBase() = delete;

    void unpackArgs(
        std::string const & func_name,
        PyPtr const & pyargs, PyPtr const & pykwds,
        OverloadResolutionData & data, bool throw_on_failure
    ) const;

    virtual void convertArgs(Registry const & registry, OverloadResolutionData & data) const = 0;

    virtual PyPtr call(Registry const & registry, OverloadResolutionData & data) const = 0;

    virtual ~OverloadBase() {}

protected:

    OverloadBase(std::size_t n_args, std::vector<std::string> kwd_names);

    std::vector<std::string> _kwd_names;
};

typedef std::unique_ptr<OverloadBase> OverloadPtr;

struct OverloadResolutionData {

    OverloadResolutionData(OverloadPtr const & p) : unpack_successful(false), overload(p.get()) {}

    bool unpack_successful;
    OverloadBase * overload;
    std::vector<PyPtr> unpacked_args;
    std::unique_ptr<ArgsFromPythonBase> converted_args;
};

template <typename Result, typename ...Args>
class Overload : public OverloadBase {

    template <int ...S>
    PyPtr _call(Registry const & registry, OverloadResolutionData & data, IntSeq<S...>) const {
        ArgsFromPython<Args...> & converted_args
            = static_cast<ArgsFromPython<Args...> &>(*data.converted_args);
        return ReturnToPython<Result>::apply(
            registry,
            _func(std::get<S>(converted_args.elements)->convert()...)
        );
    }
public:

    virtual void convertArgs(Registry const & registry, OverloadResolutionData & data) const {
        data.converted_args.reset(
            new ArgsFromPython<Args...>(registry, data.unpacked_args)
        );
    }

    virtual PyPtr call(Registry const & registry, OverloadResolutionData & data) const {
        return _call(registry, data, typename IntSeqGen<sizeof...(Args)>::Type());
    }

    Overload(std::function<Result(Args...)> func, std::vector<std::string> kwd_names) :
        OverloadBase(sizeof...(Args), std::move(kwd_names)), _func(std::move(func))
    {}

private:
    std::function<Result(Args...)> _func;
};

template <typename ...Args>
class Overload<void,Args...> : public OverloadBase {

    template <int ...S>
    PyPtr _call(Registry const & registry, OverloadResolutionData & data, IntSeq<S...>) const {
        ArgsFromPython<Args...> & converted_args
            = static_cast<ArgsFromPython<Args...> &>(*data.converted_args);
        _func(std::get<S>(converted_args.elements)->convert()...);
        return PyPtr::steal(Py_None);
    }

public:

    virtual void convertArgs(Registry const & registry, OverloadResolutionData & data) const {
        data.converted_args.reset(
            new ArgsFromPython<Args...>(registry, data.unpacked_args)
        );
    }

    virtual PyPtr call(Registry const & registry, OverloadResolutionData & data) const {
        return _call(registry, data, typename IntSeqGen<sizeof...(Args)>::Type());
    }

    Overload(std::function<void(Args...)> func, std::vector<std::string> kwd_names) :
        OverloadBase(sizeof...(Args), std::move(kwd_names)), _func(std::move(func))
    {}

private:
    std::function<void(Args...)> _func;
};

template <typename Result, typename ...Args>
inline OverloadPtr makeOverload(Result func(Args...), std::vector<std::string> kwd_names) {
    OverloadPtr p(new Overload<Result,Args...>(func, std::move(kwd_names)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline OverloadPtr makeOverload(Result (Class::*func)(Args...), std::vector<std::string> kwd_names) {
    OverloadPtr p(new Overload<Result, Class &, Args...>(func, std::move(kwd_names)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline OverloadPtr makeOverload(Result (Class::*func)(Args...) const, std::vector<std::string> kwd_names) {
    OverloadPtr p(new Overload<Result, Class const &, Args...>(func, std::move(kwd_names)));
    return p;
}

template <typename Result, typename ...Args>
inline OverloadPtr makeOverload(std::function<Result(Args...)> func, std::vector<std::string> kwd_names) {
    OverloadPtr p(new Overload<Result, Args...>(std::move(func), std::move(kwd_names)));
    return p;
}

} // namespace bp3

#endif // !BP3_Overload_hpp_INCLUDED
