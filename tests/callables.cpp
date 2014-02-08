#include "bp3/Module.hpp"
#include "bp3/Callable.hpp"

#include <iostream>
#include <string>

namespace {

static PyMethodDef methods[] = {
    {nullptr, nullptr, 0, nullptr}
};

void f1() {}

int f2(int a) { return a + 3; }

double f3(int a, double b) { return b * a; }

void wrap(bp3::Module & module) {
    bp3::Callable wf1(module.getRegistry(), "f1", f1, {});
    module.add("f1", wf1.ptr());
    bp3::Callable wf2(module.getRegistry(), "f2", f2, {"a"});
    module.add("f2", wf2.ptr());
    bp3::Callable wf3(module.getRegistry(), "f3", f3, {"a", "b"});
    module.add("f3", wf3.ptr());
    bp3::Callable wf(module.getRegistry(), "f", f1, {});
    module.add("f", wf.ptr());
    wf.addOverload(f2, {"a"});
    wf.addOverload(f3, {"a", "b"});
}

} // anonymous

BP3_MODULE(callables_mod, wrap, methods)
