#ifndef BP3_callable_hpp_INCLUDED
#define BP3_callable_hpp_INCLUDED

#include "bp3/py_ptr.hpp"
#include "bp3/keywords.hpp"
#include "bp3/conversion/args_from_python.hpp"
#include "bp3/module.hpp"

#include <functional>

namespace bp3 {

struct overload_data {
    bool unpack_successful;
    std::vector<py_ptr> unpacked_args;
    std::unique_ptr<conversion::args_from_python_base> converted_args;
};

class overload_base {
public:

    overload_base() = delete;

    void unpack_args(
        std::string const & name, py_ptr const & args, py_ptr const & kwds, overload_data & data,
        bool throw_on_failure
    ) const;

    virtual void convert_args(module const & mod, overload_data & data) const = 0;

    virtual void call(overload_data & data) const = 0;

    virtual ~overload_base() {}

protected:

    explicit overload_base(std::size_t n_args, arg_def_list const & arg_defs);

    arg_def_list _arg_defs;
};

typedef std::unique_ptr<overload_base> overload_ptr;

template <typename Result, typename ...Args>
class overload : public overload_base {
public:

    virtual void convert_args(module const & mod, overload_data & data) const {
        data.converted_args.reset(
            new conversion::args_from_python<Args...>(mod, data.unpacked_args)
        );
    }

    virtual void call(overload_data & data) const {
        
    }

    overload(std::function<Result(Args...)> func, arg_def_list arg_defs) :
        overload_base(sizeof...(Args), std::move(arg_defs)), _func(std::move(func))
    {}

private:
    std::function<Result(Args...)> _func;
};

template <typename Result, typename ...Args>
inline overload_ptr make_overload(Result func(Args...), arg_def_list arg_defs) {
    overload_ptr p(new overload<Result,Args...>(func, std::move(arg_defs)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline overload_ptr make_overload(Result (Class::*func)(Args...), arg_def_list arg_defs) {
    overload_ptr p(new overload<Result, Class &, Args...>(func, std::move(arg_defs)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline overload_ptr make_overload(Result (Class::*func)(Args...) const, arg_def_list arg_defs) {
    overload_ptr p(new overload<Result, Class const &, Args...>(func, std::move(arg_defs)));
    return p;
}

template <typename Result, typename ...Args>
inline overload_ptr make_overload(std::function<Result(Args...)> func, arg_def_list arg_defs) {
    overload_ptr p(new overload<Result, Args...>(std::move(func), std::move(arg_defs)));
    return p;
}

class callable {
public:

    py_ptr call(py_ptr const & args, py_ptr const & kwds) const;

    template <typename F>
    void add_overload(F func, arg_def_list kwargs) {
        _overloads.push_back(make_overload(std::move(func), std::move(kwargs)));
    }

    template <typename F>
    callable(
        module const & mod, std::string const & name,
        F func, arg_def_list kwargs
    ) : _name(name), _mod(mod), _overloads() {
        add_overload(func, kwargs);
    }

protected:
    std::string _name;
    module _mod;
    std::vector<overload_ptr> _overloads;
};

} // namespace bp3

#endif // !BP3_callable_hpp_INCLUDED
