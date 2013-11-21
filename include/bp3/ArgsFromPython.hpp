#ifndef BP3_ArgsFromPython_hpp_INCLUDED
#define BP3_ArgsFromPython_hpp_INCLUDED

#include "bp3/FromPython.hpp"
#include "bp3/builtin/str.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

namespace bp3 {

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

struct ArgsFromPythonBase {

    ArgsFromPythonBase(std::size_t n) : score(n) {}

    ArgsFromPythonBase(ArgsFromPythonBase const &) = delete;

    ArgsFromPythonBase & operator=(ArgsFromPythonBase const &) = delete;

    virtual void reportConversionFailure(std::ostream & os, std::string const & delimiter) const = 0;

    virtual ~ArgsFromPythonBase() {}

    OverloadScore score;
};

template <std::size_t N, typename ...E>
struct ArgsFromPythonImpl;

template <std::size_t N>
struct ArgsFromPythonImpl<N> : public ArgsFromPythonBase {

    explicit ArgsFromPythonImpl(Registry const & registryx, std::vector<PyPtr> const & unpacked_args) :
        ArgsFromPythonBase(N)
    {}

    void _reportConversionFailure(std::ostream & os, std::string const & delimiter) const {}
};

template <std::size_t N, typename T, typename ...E>
struct ArgsFromPythonImpl<N,T,E...> : public ArgsFromPythonImpl<N+1,E...> {

    typedef ArgsFromPythonImpl<N+1,E...> BaseT;

    explicit ArgsFromPythonImpl(Registry const & registry, std::vector<PyPtr> const & unpacked_args) :
        BaseT(registry, unpacked_args), _elem(registry, unpacked_args[N])
    {
        this->score.set(N, _elem.getPenalty());
    }

    void _reportConversionFailure(std::ostream & os, std::string const & delimiter) const {
        if (!_elem.isConvertible()) {
            os << delimiter
               << "cannot convert '" << std::string(builtin::repr(builtin::object(_elem.getPyObject())))
               << "' passed as argument " << N << " to C++ type '"
               << makeTypeInfo<T>().demangle() << "'";
        }
        BaseT::_reportConversionFailure(os, delimiter);
    }

    FromPython<T> _elem;
};

template <typename ...E>
class ArgsFromPython : public ArgsFromPythonImpl<0,E...> {
    typedef ArgsFromPythonImpl<0,E...> BaseT;
public:

    ArgsFromPython(Registry const & registry, std::vector<PyPtr> const & unpacked_args) :
        BaseT(registry, unpacked_args)
    {}

    virtual void reportConversionFailure(std::ostream & os, std::string const & delimiter) const {
        BaseT::_reportConversionFailure(os, delimiter);
    }

};

template <std::size_t N, typename A>
struct NthArg;

template <std::size_t N, typename T, typename ...E>
struct NthArg<N, ArgsFromPython<T,E...>> : public NthArg<N-1, ArgsFromPython<E...>> {};

template <typename T, typename ...E>
struct NthArg<0, ArgsFromPython<T,E...>> {
    typedef T Type;
};

} // namespace bp3

#endif // !BP3_ArgsFromPython_hpp_INCLUDED
