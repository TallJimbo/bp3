#ifndef BP3_ArgsFromPython_hpp_INCLUDED
#define BP3_ArgsFromPython_hpp_INCLUDED

#include "bp3/FromPython.hpp"
#include "bp3/builtin/str.hpp"

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>

namespace bp3 {

// TODO: move these metaprogramming utilities to a separate header

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

struct OverloadScore {

    explicit OverloadScore(std::size_t n) : _penalties(n, -1) {}

    void set(std::size_t n, int penalty) { _penalties[n] = penalty; }

    bool isValid() const {
        return std::all_of(_penalties.begin(), _penalties.end(), [](int p){ return p >= 0; });
    }

    int compare(OverloadScore const & other) const {
        assert(isValid() && other.isValid());
        std::size_t stop = std::min(_penalties.size(), other._penalties.size());
        int result = 0;
        if (_penalties.size() > other._penalties.size()) {
            result = -1;
        } else if (_penalties.size() < other._penalties.size()) {
            result = 1;
        }
        for (std::size_t i = 0; i < stop; ++i) {
            if (_penalties[i] < other._penalties[i]) {
                if (result > 0) {
                    return 0; // better in one arg, worse in another = indeterminant
                } else if (result == 0) {
                    result = -1;
                }
            } else if (_penalties[i] > other._penalties[i]) {
                if (result < 0) {
                    return 0; // better in one arg, worse in another = indeterminant
                } else if (result == 0) {
                    result = 1;
                }
            }
        }
        return result;
    }

private:
    std::vector<int> _penalties;
};

class ArgsFromPythonBase {
protected:

    struct ReportConversionFailure {

        template <typename T>
        void operator()(std::shared_ptr< FromPython<T> > const & element) {
            if (!element->isConvertible()) {
                os << delimiter
                   << "cannot convert '"
                   << std::string(builtin::repr(builtin::object(element->getPyObject())))
                   << "' passed as argument " << ++n << " to C++ type '"
                   << makeTypeInfo<T>().demangle() << "'";
            }
        }

        ReportConversionFailure(std::ostream & os_, std::string const & delimiter_) :
            n(0), os(os_), delimiter(delimiter_)
        {}

        int n;
        std::ostream & os;
        std::string const & delimiter;
    };

public:

    ArgsFromPythonBase(std::size_t n) : score(n) {}

    ArgsFromPythonBase(ArgsFromPythonBase const &) = delete;

    ArgsFromPythonBase & operator=(ArgsFromPythonBase const &) = delete;

    virtual void reportConversionFailure(std::ostream & os, std::string const & delimiter) const = 0;

    virtual ~ArgsFromPythonBase() {}

    OverloadScore score;
};

template <std::size_t N, typename ...E>
struct ArgsFromPythonHelper;

template <std::size_t N>
struct ArgsFromPythonHelper<N> {

    static std::tuple<>
    apply(OverloadScore & score, Registry const & registry, std::vector<PyPtr> const & unpacked_args) {}

};

template <std::size_t N, typename T, typename ...E>
struct ArgsFromPythonHelper<N,T,E...> {

    // TODO: we probably don't need shared_ptrs here, as we should be able to use move semantics
    // instead.  But as of g++ 4.6 that doesn't seem to be working - we should try again with a future
    // version.
    static std::tuple<std::shared_ptr<FromPython<T>>,std::shared_ptr<FromPython<E>>...>
    apply(OverloadScore & score, Registry const & registry, std::vector<PyPtr> const & unpacked_args) {
        std::shared_ptr<FromPython<T>> element = std::make_shared<FromPython<T>>(registry, unpacked_args[N]);
        score.set(N, element->getPenalty());
        std::tuple<std::shared_ptr<FromPython<T>>,std::shared_ptr<FromPython<E>>...> r
            = std::tuple_cat(
                std::make_tuple(element),
                ArgsFromPythonHelper<N+1,E...>::apply(score, registry, unpacked_args)
            );
        return r;
    }

};

template <typename ...E>
class ArgsFromPython : public ArgsFromPythonBase {
    typedef std::tuple<std::shared_ptr<FromPython<E>>...> Tuple;
public:

    ArgsFromPython(Registry const & registry, std::vector<PyPtr> const & unpacked_args) :
        ArgsFromPythonBase(sizeof...(E)),
        elements(ArgsFromPythonHelper<0,E...>::apply(this->score, registry, unpacked_args))
    {}

    virtual void reportConversionFailure(std::ostream & os, std::string const & delimiter) const {
        ArgsFromPythonBase::ReportConversionFailure f(os, delimiter);
        forEach(f, elements);
    }

    Tuple elements;
};

} // namespace bp3

#endif // !BP3_ArgsFromPython_hpp_INCLUDED
