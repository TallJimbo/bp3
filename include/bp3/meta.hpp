#ifndef BP3_meta_hpp_INCLUDED
#define BP3_meta_hpp_INCLUDED

#include <tuple>

namespace bp3 {
namespace meta {

template <typename ...E>
struct vector {
    template <typename V>
    struct push_front {
        typedef vector<V,E...> type;
    };
    typedef std::tuple<E...> as_tuple;
};

template <typename V>
struct as_tuple : public as_tuple<typename V::type> {};

template <typename ...E>
struct as_tuple<vector<E...>> {
    typedef std::tuple<E...> type;
};

template <typename ...E>
struct for_each {};

template <typename V, typename ...E>
struct for_each<V,E...> {
    template <typename F>
    static bool apply(F && f) {
        return f.template apply<V>() && for_each<E...>::apply(std::forward<F>(f));
    }
};

template <>
struct for_each<> {
    template <typename F> static bool apply(F && f) { return true; };
};

template <typename ...E>
struct for_each<vector<E...>> : public for_each<E...> {};

template <typename ...E>
struct transform_impl {};

template <typename V, typename ...E>
struct transform_impl<V,E...> {
    template <typename F>
    struct apply {
        typedef typename transform_impl<E...>::template apply<F>::type::template 
        push_front<typename F::template apply<V>::type>::type type;
    };
};

template <>
struct transform_impl<> {
    template <typename F>
    struct apply {
        typedef vector<> type;
    };
};

template <typename F, typename ...E>
struct transform {
    typedef typename transform_impl<E...>::template apply<F>::type type;
};

template <std::size_t N,std::size_t I=0>
struct for_each_n_impl {
    template <typename F>
    static bool apply(F && f) {
        return f.apply<I>() && for_each_n_impl<N,I+1>::apply(f);
    }
};

template <std::size_t N>
struct for_each_n_impl<N,N> {
    template <typename F>
    static bool apply(F && f) { return true; }
};

template <std::size_t N, typename F>
bool for_each_n(F && f) {
    return for_each_n_impl<N>::apply(f);
}

} // namespace meta
} // namespace bp3

#endif // !BP3_meta_hpp_INCLUDED
