#ifndef BP3_keywords_hpp_INCLUDED
#define BP3_keywords_hpp_INCLUDED

#include "bp3/py_ptr.hpp"
#include "bp3/meta.hpp"
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

class kwds {
public:

    typedef std::vector<arg>::const_iterator iterator;

    kwds(std::initializer_list<arg> args) : _args(args) {}

    void parse(
        std::string const & function_name,
        py_ptr const & args, py_ptr const & kwds,
        std::vector<py_ptr> & output
    ) const;

    iterator begin() const { return _args.begin(); }
    iterator end() const { return _args.end(); }

private:
    std::vector<arg> _args;
};

} // namespace bp3

#endif // !BP3_keywords_hpp_INCLUDED
