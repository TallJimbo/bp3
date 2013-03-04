#ifndef BP3_callable_hpp_INCLUDED
#define BP3_callable_hpp_INCLUDED

#include <functional>
#include <list>
#include <typeinfo>
#include <iostream>

#include "bp3/meta.hpp"
#include "bp3/py_ptr.hpp"
#include "bp3/keywords.hpp"
#include "bp3/conversion/args_from_python.hpp"
#include "bp3/context.hpp"

namespace bp3 {

class callable_base {
public:

    callable_base() = delete;

    virtual py_ptr call(py_ptr const & args, py_ptr const & kwds) const = 0;

    virtual ~callable_base() {}

protected:

    explicit callable_base(context_t & context, std::string const & name, arg_list const & args) :
        _name(name), _args(args), _context(&context)
    {}

    std::string _name;
    arg_list _args;
    context_t * _context;
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

    virtual py_ptr call(py_ptr const & args, py_ptr const & kwds) const {
        try {
            std::vector<py_ptr> unpacked_args(sizeof...(Args));
            _args.parse(_name, args, kwds, unpacked_args);
            conversion::args_from_python<Args...> converted_args(*_context, unpacked_args);
        } catch (builtin::Exception & err) {
            err.release();
            return py_ptr();
        } catch (...) {
            // TODO: to-Python conversion of C++ exceptions, throw a custom exception otherwise
            PyErr_SetString(PyExc_RuntimeError, "Unknown C++ exception");
            return py_ptr();
        }
        return py_ptr::borrow(Py_None);
    }

    callable(
        context_t & context,
        std::string const & name,
        std::function<Result(Args...)> const & func,
        arg_list const & args
    ) :
        callable_base(context, name, args), _func(func)
    {}

private:
    std::function<Result(Args...)> _func;
};

template <typename Result, typename ...Args>
callable<Result, Args...> def(
    context_t & context,
    std::string const & name,
    Result func(Args...),
    arg_list const & args
) {
    return callable<Result, Args...>(context, name, func, args);
}

template <typename Result, typename Class, typename ...Args>
callable<Result, Class &, Args...> def(
    context_t & context,
    std::string const & name,
    Result (Class::*func)(Args...),
    arg_list const & args
) {
    return callable<Result, Class &, Args...>(context, name, func, args);
}

template <typename Result, typename Class, typename ...Args>
callable<Result, Class const &, Args...> def(
    context_t & context,
    std::string const & name,
    Result (Class::*func)(Args...) const,
    arg_list const & args
) {
    return callable<Result, Class const &, Args...>(context, name, func, args);
}

template <typename Result, typename ...Args>
callable<Result, Args...> def(
    context_t & context,
    std::string const & name,
    std::function<Result(Args...)> func,
    arg_list const & args
) {
    return callable<Result, Args...>(context, name, func, args);
}

} // namespace bp3

#endif // !BP3_callable_hpp_INCLUDED
