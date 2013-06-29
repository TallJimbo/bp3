#ifndef BP3_CALLABLES_ArgsFromPython_hpp_INCLUDED
#define BP3_CALLABLES_ArgsFromPython_hpp_INCLUDED

#include "bp3/FromPython.hpp"

namespace bp3 { namespace callables {

struct ArgsFromPythonBase {

    ArgsFromPythonBase() : penalty(0) {}

    ArgsFromPythonBase(ArgsFromPythonBase const &) = delete;

    ArgsFromPythonBase & operator=(ArgsFromPythonBase const &) = delete;

    virtual ~ArgsFromPythonBase() {}

    int penalty;
};

template <std::size_t N, typename ...E>
struct ArgsFromPythonImpl;

template <std::size_t N>
struct ArgsFromPythonImpl<N> : public ArgsFromPythonBase {

    explicit ArgsFromPythonImpl(Module const & mod, std::vector<PyPtr> const & unpacked_args) {}

};

template <std::size_t N, typename T, typename ...E>
struct ArgsFromPythonImpl<N,T,E...> : public ArgsFromPythonImpl<N+1,E...> {

    typedef ArgsFromPythonImpl<N+1,E...> base_t;

    explicit ArgsFromPythonImpl(Module const & mod, std::vector<PyPtr> const & unpacked_args) :
        base_t(mod, unpacked_args), _elem(mod, unpacked_args[N])
    {
        this->penalty += _elem.penalty();
    }

    FromPython<T> _elem;
};

template <typename ...E>
class ArgsFromPython : public ArgsFromPythonImpl<0,E...> {
    typedef ArgsFromPythonImpl<0,E...> base_t;
public:

    ArgsFromPython(Module const & mod, std::vector<PyPtr> const & unpacked_args) :
        base_t(mod, unpacked_args)
    {}

};

template <std::size_t N, typename A>
struct NthArg;

template <std::size_t N, typename T, typename ...E>
struct NthArg<N, ArgsFromPython<T,E...>> : public NthArg<N-1, ArgsFromPython<E...>> {};

template <typename T, typename ...E>
struct NthArg<0, ArgsFromPython<T,E...>> {
    typedef T Type;
};

}} // namespace bp3::callables

#endif // !BP3_CALLABLES_ArgsFromPython_hpp_INCLUDED
