#ifndef BP3_callable_hpp_INCLUDED
#define BP3_callable_hpp_INCLUDED

#include "bp3/py_ptr.hpp"
#include "bp3/meta.hpp"
#include "bp3/keywords.hpp"
#include "bp3/conversion/args_from_python.hpp"
#include "bp3/context.hpp"

#include <functional>

namespace bp3 {

class overload_base {
public:

    overload_base() = delete;

    virtual ~overload_base() {}

protected:

    friend class callable;

    explicit overload_base(std::size_t n_args, arg_list const & kwargs);

    arg_list _kwargs;
};

typedef std::unique_ptr<overload_base> overload_ptr;

template <typename Result, typename ...Args>
class overload : public overload_base {
public:

    overload(std::function<Result(Args...)> func, arg_list kwargs) :
        overload_base(sizeof...(Args), std::move(kwargs)), _func(std::move(func))
    {}

private:
    std::function<Result(Args...)> _func;
};

template <typename Result, typename ...Args>
inline overload_ptr make_overload(Result func(Args...), arg_list kwargs) {
    overload_ptr p(new overload<Result,Args...>(func, std::move(kwargs)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline overload_ptr make_overload(Result (Class::*func)(Args...), arg_list kwargs) {
    overload_ptr p(new overload<Result, Class &, Args...>(func, std::move(kwargs)));
    return p;
}

template <typename Result, typename Class, typename ...Args>
inline overload_ptr make_overload(Result (Class::*func)(Args...) const, arg_list kwargs) {
    overload_ptr p(new overload<Result, Class const &, Args...>(func, std::move(kwargs)));
    return p;
}

template <typename Result, typename ...Args>
inline overload_ptr make_overload(std::function<Result(Args...)> func, arg_list kwargs) {
    overload_ptr p(new overload<Result, Args...>(std::move(func), std::move(kwargs)));
    return p;
}

class callable {
public:

    py_ptr call(py_ptr const & args, py_ptr const & kwds) const;

    template <typename F>
    void add_overload(F func, arg_list kwargs) {
        _overloads.push_back(make_overload(std::move(func), std::move(kwargs)));
    }

    template <typename F>
    callable(
        context_t const & context, std::string const & name,
        F func, arg_list kwargs
    ) : _name(name), _context(&context), _overloads() {
        add_overload(func, kwargs);
    }

protected:
    std::string _name;
    context_t const * _context;
    std::vector<overload_ptr> _overloads;
};

} // namespace bp3

#endif // !BP3_callable_hpp_INCLUDED
