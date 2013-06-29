#ifndef BP3_ArgDef_hpp_INCLUDED
#define BP3_ArgDef_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/builtin/exceptions.hpp"
#include "bp3/utils/Any.hpp"

#include <vector>
#include <iostream>

namespace bp3 {
namespace callables {

class ArgDef {
public:
    std::string name;
    utils::Any value;

    ArgDef(std::string s) : name(s) {}

    template <typename T>
    ArgDef(std::string s, T v) : name(s), value(v) {}

    template <typename T>
    ArgDef operator=(T v) const { return ArgDef(name, v); }
};

class ArgDefList {
public:

    ArgDefList(std::initializer_list<ArgDef> x) : _vec(x) {}

    bool parse(
        std::string const & function_name,
        PyPtr const & args, PyPtr const & kwds,
        std::vector<PyPtr> & output,
        bool throw_on_failure=true
    ) const;

    void validate(std::size_t n_args);

private:
    std::vector<ArgDef> _vec;
};

} // namespace callables

inline callables::ArgDef arg(std::string name) { return callables::ArgDef(name); }

template <typename T>
inline callables::ArgDef arg(std::string name, T value) { return callables::ArgDef(name, value); }

} // namespace bp3

#endif // !BP3_ArgDef_hpp_INCLUDED
