#include "bp3/Module.hpp"

#include <iostream>
#include <string>

namespace bp3 { namespace std {

void initExceptions(Module & module);

namespace {

static PyMethodDef methods[] = {
    {nullptr, nullptr, 0, nullptr}
};

void wrap(bp3::Module & module) {
    initExceptions(module);
}

} // anonymous

}} // namespace bp3::std

BP3_MODULE(_std, bp3::std::wrap, bp3::std::methods)
