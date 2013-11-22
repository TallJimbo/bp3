#include "bp3/Module.hpp"
#include "bp3/Registration.hpp"
#include "bp3/builtin/str.hpp"

#include <cstring>

namespace bp3 {

Module::Module(PyPtr const & ptr) :
    _ptr(ptr), _registry(*this, true)
{}

Module::Module(std::string const & name) :
    _ptr(PyPtr::steal(PyImport_Import(builtin::str(name).ptr().get())).raise_if_null()),
    _registry(*this, false)
{}

Module Module::import(std::string const & name) const {
    Module module(name);
    getRegistry().import(module.getRegistry());
    return module;
}

void Module::add(std::string const & name, PyPtr const & ptr) const {
    if (PyModule_AddObject(_ptr.get(), name.c_str(), ptr.incref()) < 0) {
        throw_error_already_set();
    }
}

} // namespace bp3
