#ifndef BP3_keywords_hpp_INCLUDED
#define BP3_keywords_hpp_INCLUDED

#include "bp3/py_ptr.hpp"
#include "bp3/builtin/exceptions.hpp"
#include "bp3/utils/any.hpp"

#include <vector>
#include <iostream>

namespace bp3 {

class arg {
public:
    std::string name;
    utils::any value;

    arg(char const * s) : name(s) {}

    arg(std::string const & s) : name(s) {}

    template <typename T>
    arg(std::string const & s, T v) : name(s), value(v) {}

    template <typename T>
    arg operator=(T v) { return arg{name, v}; }
};

class arg_list {
public:

    arg_list(std::initializer_list<arg> x) : _vec(x) {}

    bool parse(
        std::string const & function_name,
        py_ptr const & args, py_ptr const & arg_list,
        std::vector<py_ptr> & output,
        bool throw_on_failure=true
    ) const;

    void validate(std::size_t n_args);

private:
    std::vector<arg> _vec;
};

} // namespace bp3

#endif // !BP3_keywords_hpp_INCLUDED
