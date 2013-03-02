#ifndef BP3_callable_hpp_INCLUDED
#define BP3_callable_hpp_INCLUDED

#include <functional>
#include <list>
#include <typeinfo>
#include <iostream>

#include "bp3/meta.hpp"
#include "bp3/py_ptr.hpp"
#include "bp3/keywords.hpp"

namespace bp3 {

class callable_base {
public:

    callable_base() = delete;

    virtual void call(py_ptr const & args, py_ptr const & kwds) const = 0;

    virtual ~callable_base() {}

protected:

    explicit callable_base(std::string const & name, arg_list const & args) : _name(name), _args(args) {}

    std::string _name;
    arg_list _args;
};

struct print_type {
    template <typename T>
    bool apply() {
        std::cerr << i << ": " << typeid(T).name() << "\n";
        ++i;
        return true;
    }
    int i;
};

template <typename Result, typename ...Args>
class callable : public callable_base {
public:

    virtual void call(py_ptr const & args, py_ptr const & kwds) const {
        std::vector<py_ptr> unpacked_args;
        _args.parse(_name, args, kwds, unpacked_args);
    }

    callable(std::string const & name, std::function<Result(Args...)> const & func, arg_list const & args) :
        callable_base(name, args), _func(func)
    {}

private:
    std::function<Result(Args...)> _func;
};

template <typename Result, typename ...Args>
callable<Result, Args...> def(
    std::string const & name,
    Result func(Args...),
    arg_list const & args
) {
    return callable<Result, Args...>(name, func, args);
}

template <typename Result, typename Class, typename ...Args>
callable<Result, Class &, Args...> def(
    std::string const & name,
    Result (Class::*func)(Args...),
    arg_list const & args
) {
    return callable<Result, Class &, Args...>(name, func, args);
}

template <typename Result, typename Class, typename ...Args>
callable<Result, Class const &, Args...> def(
    std::string const & name,
    Result (Class::*func)(Args...) const,
    arg_list const & args
) {
    return callable<Result, Class const &, Args...>(name, func, args);
}

template <typename Result, typename ...Args>
callable<Result, Args...> def(
    std::string const & name,
    std::function<Result(Args...)> func,
    arg_list const & args
) {
    return callable<Result, Args...>(name, func, args);
}

} // namespace bp3

#endif // !BP3_callable_hpp_INCLUDED
