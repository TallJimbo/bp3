#include "bp3/Module.hpp"
#include "bp3/callables/Callable.hpp"

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
    bp3::callables::Callable wf1(mod, "f1", f1, {});
    bp3::callables::Callable wf2(mod, "f2", f2, {"a"});
    bp3::callables::Callable wf3(mod, "f3", f3, {"a", "b"});
    bp3::callables::Callable wf(mod, "f", f1, {});
    wf.addOverload(f2, {"a"});
    wf.addOverload(f3, {"a", "b"});
    bp3::PyPtr kwds = bp3::PyPtr::steal(PyDict_New());
    wf1.call(bp3::builtin::tuple().ptr(), kwds);
}

} // anonymous

BP3_MODULE(callables_mod, wrap, methods)
