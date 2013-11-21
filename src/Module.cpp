#include "bp3/Module.hpp"
#include "bp3/Registration.hpp"
#include "bp3/builtin/numbers.hpp"

#include <cstring>

namespace bp3 {

Module::Module(char const * name, PyPtr const & ptr) :
    _ptr(ptr), _registry()
{
    add("__bp3_registry__", _registry.ptr());
}

void Module::add(std::string const & name, PyPtr const & ptr) const {
    if (PyModule_AddObject(_ptr.get(), name.c_str(), ptr.incref()) < 0) {
        throw_error_already_set();
    }
}

} // namespace bp3
