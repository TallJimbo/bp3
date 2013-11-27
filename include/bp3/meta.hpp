#ifndef BP3_meta_hpp_INCLUDED
#define BP3_meta_hpp_INCLUDED

#include <tuple>
#include <algorithm>

namespace bp3 {

template <int ...S> struct IntSeq {};

template <int N, int ...S> struct IntSeqGen : IntSeqGen<N-1, N-1, S...> {};

template <int ...S> struct IntSeqGen<0, S...>{ typedef IntSeq<S...> Type; };

template <typename Func, typename ...E1>
struct ForEachHelper {

    void apply() {
        _apply1<0,E1...>();
    }

    ForEachHelper(Func & f, std::tuple<E1...> const & t) : _f(f), _t(t) {}

private:

    template <int>
    void _apply1() {}

    template <int, typename T, typename ...E2>
    void _apply1() {
        _apply1<0,E2...>();
        _apply2<sizeof...(E2)>();
    }

    template <std::size_t N>
    void _apply2() {
        _f(std::get<N>(_t));
    }

    Func & _f;
    std::tuple<E1...> const & _t;
};

template <typename Func, typename ...E>
void forEach(Func & f, std::tuple<E...> const & t) {
    ForEachHelper<Func,E...>(f, t).apply();
}

} // namespace bp3

#endif // !BP3_meta_hpp_INCLUDED
