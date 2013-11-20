#ifndef BP3_CALLABLES_Callable_hpp_INCLUDED
#define BP3_CALLABLES_Callable_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/callables/Overload.hpp"
#include "bp3/callables/ArgsFromPython.hpp"
#include "bp3/Module.hpp"
#include "bp3/library.hpp"

#include <functional>

namespace bp3 {
namespace callables {

class Callable {
public:

    PyPtr call(PyPtr const & args, PyPtr const & kwds) const;

    template <typename F>
    void addOverload(F func, std::vector<std::string> kwd_names) {
        _addOverload(makeOverload(std::move(func), std::move(kwd_names)));
    }

    template <typename F>
    Callable(
        Module mod, std::string name,
        F func, std::vector<std::string> kwd_names
    ) {
        _initialize(std::move(mod), std::move(name));
        addOverload(func, std::move(kwd_names));
    }

    PyPtr const & ptr() const { return _ptr; }

private:

    friend class bp3::LibraryAccess;

    static PyPtr initType();

    void _initialize(Module mod, std::string name);
    void _addOverload(OverloadPtr overload);

    PyPtr _ptr;
};

} // namespace callables
} // namespace bp3

#endif // !BP3_CALLABLES_Callable_hpp_INCLUDED
