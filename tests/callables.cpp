#include "bp3/Module.hpp"
#include "bp3/Callable.hpp"

#include <iostream>
#include <string>

namespace {

static PyMethodDef methods[] = {
    {nullptr, nullptr, 0, nullptr}
};

void f1() {}

void f2(int a) {}

void f3(int a, double b) {}

void wrap(bp3::Module & mod) {
    bp3::Callable wf1(mod.getRegistry(), "f1", f1, {});
    bp3::Callable wf2(mod.getRegistry(), "f2", f2, {"a"});
    bp3::Callable wf3(mod.getRegistry(), "f3", f3, {"a", "b"});
    bp3::Callable wf(mod.getRegistry(), "f", f1, {});
    wf.addOverload(f2, {"a"});
    wf.addOverload(f3, {"a", "b"});
    bp3::PyPtr kwds = bp3::PyPtr::steal(PyDict_New());
    wf1.call(bp3::builtin::tuple().ptr(), kwds);
}

} // anonymous

BP3_MODULE(callables_mod, wrap, methods)
