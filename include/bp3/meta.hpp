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
    static void apply(F && f) {
        f.template apply<V>();
        for_each<E...>::apply(std::forward<F>(f));
    }
};

template <>
struct for_each<> {
    template <typename F> static void apply(F && f) {};
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

template <std::size_t I, typename T, std::size_t N=std::tuple_size<T>::value-1>
struct call_on_tuple {
    template <typename F>
    static void apply(F && f, T const & t) {
        f(std::get<I>(t));
        call_on_tuple<I+1,T>::apply(f, t);
    }    
};

template <std::size_t I, typename T>
struct call_on_tuple<I,T,I> {
    template <typename F>
    static void apply(F && f, T const & t) {
        f(std::get<I>(t));
    }
};

template <typename F, typename T>
void for_each_item(F && f, T const & t) {
    call_on_tuple<0,T>::apply(f, t);
}


} // namespace meta
} // namespace bp3

#endif // !BP3_meta_hpp_INCLUDED
