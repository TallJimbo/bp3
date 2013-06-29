#ifndef BP3_CALLABLES_Callable_hpp_INCLUDED
#define BP3_CALLABLES_Callable_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/callables/ArgDef.hpp"
#include "bp3/callables/Overload.hpp"
#include "bp3/callables/ArgsFromPython.hpp"
#include "bp3/Module.hpp"

#include <functional>

namespace bp3 {
namespace callables {

class Callable {
public:

    PyPtr call(PyPtr const & args, PyPtr const & kwds) const;

    template <typename F>
    void addOverload(F func, ArgDefList kwargs) {
        _overloads.push_back(make_overload(std::move(func), std::move(kwargs)));
    }

    template <typename F>
    Callable(
        Module const & mod, std::string const & name,
        F func, ArgDefList kwargs
    ) : _name(name), _mod(mod), _overloads() {
        addOverload(func, kwargs);
    }

protected:
    std::string _name;
    Module _mod;
    std::vector<OverloadPtr> _overloads;
};

} // namespace callables
} // namespace bp3

#endif // !BP3_CALLABLES_Callable_hpp_INCLUDED
