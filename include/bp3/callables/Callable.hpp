#ifndef BP3_CALLABLES_Callable_hpp_INCLUDED
#define BP3_CALLABLES_Callable_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
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
    void addOverload(F func, std::vector<std::string> kwd_names) {
        _overloads.push_back(makeOverload(std::move(func), std::move(kwd_names)));
    }

    template <typename F>
    Callable(
        Module const & mod, std::string const & name,
        F func, std::vector<std::string> kwd_names
    ) : _name(name), _mod(mod), _overloads() {
        addOverload(func, kwd_names);
    }

protected:
    std::string _name;
    Module _mod;
    std::vector<OverloadPtr> _overloads;
};

} // namespace callables
} // namespace bp3

#endif // !BP3_CALLABLES_Callable_hpp_INCLUDED
