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
    template <typename F>
    static void apply(F && f) {};
};

template <typename ...E>
struct for_each<vector<E...>> : public for_each<E...> {};

template <typename ...E>
struct transform {};

template <typename V, typename ...E>
struct transform<V,E...> {
    template <typename F>
    struct apply {
        typedef typename transform<E...>::template apply<F>::type::template push_front<V>::type type;
    };
};

template <>
struct transform<> {
    template <typename F>
    struct apply {
        typedef vector<> type;
    };
};

} // namespace meta
} // namespace bp3

#endif // !BP3_meta_hpp_INCLUDED
