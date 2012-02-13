#ifndef BP3_callable_hpp_INCLUDED
#define BP3_callable_hpp_INCLUDED

#include <functional>
#include <list>
#include <typeinfo>
#include <iostream>

#include "bp3/meta.hpp"

namespace bp3 {

class callable_base {
public:

    virtual void call() const = 0;

    virtual ~callable_base() {}

};

struct print_type {
    template <typename T>
    void apply() {
        std::cerr << i << ": " << typeid(T).name() << "\n";
        ++i;
    }
    int i;
};

template <typename Result, typename ...Args>
class callable : public callable_base {
public:

    virtual void call() const {
        print_type printer = { 0 };
        meta::for_each<Args...>::apply(printer);
    }

    explicit callable(std::function<Result(Args...)> const & func) : _func(func) {}

private:
    std::function<Result(Args...)> _func;
};

template <typename Result, typename ...Args>
callable<Result, Args...> def(Result func(Args...)) {
    return callable<Result, Args...>(func);
}

template <typename Result, typename Class, typename ...Args>
callable<Result, Class &, Args...> def(Result (Class::*func)(Args...)) {
    return callable<Result, Class &, Args...>(func);
}

template <typename Result, typename Class, typename ...Args>
callable<Result, Class const &, Args...> def(Result (Class::*func)(Args...) const) {
    return callable<Result, Class const &, Args...>(func);
}

template <typename Result, typename ...Args>
callable<Result, Args...> def(std::function<Result(Args...)> func) {
    return callable<Result, Args...>(func);
}

} // namespace bp3

#endif // !BP3_callable_hpp_INCLUDED
