#include "bp3/Module.hpp"
#include "bp3/callables/Callable.hpp"

#include <iostream>
#include <string>

namespace bp3 {

class LibraryAccess {
public:

    static void initTypes(Module & module) {
        module.add("Callable", bp3::callables::Callable::initType());
    }
};

} // namespace bp3

namespace {

static PyMethodDef methods[] = {
    {nullptr, nullptr, 0, nullptr}
};

void wrap(bp3::Module & module) {
    bp3::LibraryAccess::initTypes(module);
}

} // anonymous

BP3_MODULE(_bp3, wrap, methods)
