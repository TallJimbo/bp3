#ifndef BP3_keywords_hpp_INCLUDED
#define BP3_keywords_hpp_INCLUDED

#include "bp3/py_ptr.hpp"
#include "bp3/meta.hpp"
#include "bp3/builtin/exceptions.hpp"

#include <array>
#include <iostream>

namespace bp3 {

namespace kw {

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

bool start_pack_parse(
    char const ** names, std::size_t const len_names,
    py_ptr const & args, py_ptr const & kwds,
    py_ptr * & output, std::size_t const len_output
);

template <typename ...E>
class pack {

    typedef std::tuple<E...> data_t;
    typedef std::tuple_size<data_t> size;

    struct stream_functor {

        template <std::size_t I>
        bool apply() {
            apply(std::get<I>(data));
            return true;
        }
        template <typename T>
        void apply(arg_t<T> const & a) {
            os << a.name << "=" << a.value << ", ";
        }
        void apply(arg_t<no_default> const & a) {
            os << a.name << ", ";
        }

        data_t const & data;
        std::ostream & os;
    };

    struct get_names_functor {

        template <std::size_t I>
        bool apply() {
            names[I] = std::get<I>(data).name;
            return true;
        }

        data_t const & data;
        std::array<char const *,size::value> & names;
    };

    // checks the output args to see if we have defaults for the ones that weren't provided
    struct check_parse_functor {

        template <std::size_t I>
        bool apply() {
            return apply(I, std::get<I>(data));
        }

        template <typename T>
        bool apply(std::size_t i, arg_t<T> const &) { return true; }

        bool apply(std::size_t i, arg_t<no_default> const &) { return bool(output[i]); }

        data_t const & data;
        py_ptr * output;
    };

public:

    explicit pack(E...args) : _data(args...) {}

    template <std::size_t N>
    bool parse(
        py_ptr const & args, py_ptr const & kwds,
        std::array<py_ptr,N> & output
    ) const {
        static_assert(
            size::value <= N,
            "too many keyword arguments specified"
        );
        static_assert(
            N <= size::value + 1,
            "not enough keyword arguments specified"
        );
        std::array<char const *,size::value> names = get_names();
        py_ptr * output_p = output.data();
        if (!start_pack_parse(names.data(), size::value, args, kwds, output_p, N)) return false;
        check_parse_functor f{_data, output_p};
        return meta::for_each_n<size::value>(f);
    }

    friend std::ostream & operator<<(std::ostream & os, pack<E...> const & p) {
        stream_functor f{p._data, os};
        meta::for_each_n<size::value>(f);
        return os;
    }

    std::array<char const *,size::value> get_names() const {
        std::array<char const *,size::value> result;
        get_names_functor f{_data, result};
        meta::for_each_n<size::value>(f);
        return result;
    }

private:
    data_t  _data;
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

} // namespace kw

inline kw::arg_t<> arg(char const * name) {
    return kw::arg_t<>{name};
}

template <typename ...E>
typename kw::as_pack<meta::transform<kw::make_arg,E...>>::type
kwds(E...args) {
    return typename kw::as_pack<meta::transform<kw::make_arg,E...>>::type(args...);
}

} // namespace bp3

#endif // !BP3_keywords_hpp_INCLUDED
