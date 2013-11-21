#ifndef BP3_Callable_hpp_INCLUDED
#define BP3_Callable_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/Overload.hpp"
#include "bp3/ArgsFromPython.hpp"
#include "bp3/Registry.hpp"
#include "bp3/library.hpp"

#include <functional>

namespace bp3 {

class Callable {
public:

    PyPtr call(PyPtr const & args, PyPtr const & kwds) const;

    template <typename F>
    void addOverload(F func, std::vector<std::string> kwd_names) {
        _addOverload(makeOverload(std::move(func), std::move(kwd_names)));
    }

    template <typename F>
    Callable(
        Registry registry, std::string name,
        F func, std::vector<std::string> kwd_names
    ) {
        _initialize(std::move(registry), std::move(name));
        addOverload(func, std::move(kwd_names));
    }

    PyPtr const & ptr() const { return _ptr; }

private:

    friend class bp3::LibraryAccess;

    static PyPtr initType();

    void _initialize(Registry registry, std::string name);
    void _addOverload(OverloadPtr overload);

    PyPtr _ptr;
};

} // namespace bp3

#endif // !BP3_Callable_hpp_INCLUDED
