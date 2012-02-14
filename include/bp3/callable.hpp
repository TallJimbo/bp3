#ifndef BP3_callable_hpp_INCLUDED
#define BP3_callable_hpp_INCLUDED

#include <functional>
#include <list>
#include <typeinfo>
#include <iostream>

#include "bp3/meta.hpp"
#include "bp3/py_ptr.hpp"

namespace bp3 {

namespace kwd {

struct no_default {};

template <typename T=no_default>
struct arg_t {
    char const * name;
    T value;
};

template <>
struct arg_t<no_default> {
    char const * name;

    arg_t(char const * s) : name(s) {}

    template <typename T>
    arg_t<T> operator=(T const & value) { return arg_t<T>{name, value}; }
};

template <typename ...E>
struct pack {

    explicit pack(E...args) : data(args...) {}

    std::tuple<E...> data;
};

struct make_arg {
    template <typename T>
    struct apply {
        typedef arg_t<no_default> type;
    };
};

template <typename T>
struct make_arg::apply<arg_t<T>> {
    typedef arg_t<T> type;
};

template <typename T>
struct as_pack : public as_pack<typename T::type> {};

template <typename ...E>
struct as_pack<meta::vector<E...>> {
    typedef pack<E...> type;
};

struct stream_args {
    template <typename T>
    void operator()(arg_t<T> const & a) {
        os << a.name << "=" << a.value << ", ";
    }
    void operator()(arg_t<no_default> const & a) {
        os << a.name << ", ";
    }

    std::ostream & os;
};

template <typename ...E>
std::ostream & operator<<(std::ostream & os, pack<E...> const & p) {
    stream_args f{os};
    meta::for_each_item(f, p.data);
}

} // namespace kwd

inline kwd::arg_t<> arg(char const * name) {
    return kwd::arg_t<>{name};
}

template <typename ...E>
typename kwd::as_pack<meta::transform<kwd::make_arg,E...>>::type
kwds(E...args) {
    return typename kwd::as_pack<meta::transform<kwd::make_arg,E...>>::type(args...);
}

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

    virtual void call(void) const {
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
