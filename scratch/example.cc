#include "bp3/py_ptr.hpp"
#include "bp3/callable.hpp"

class Example {
public:

    int func1(double) { return 1; }
    int func2(double) const { return 2; }

};

int func3(Example &, double) { return 3; }

class FuncN {
public:

    int operator()(Example &, double) const { return n; }

    int n;

};

PyObject * run(PyObject *, PyObject *) {

    FuncN func4 = { 4 };

    auto f1 = bp3::def(&Example::func1);
    auto f2 = bp3::def(&Example::func2);
    auto f3 = bp3::def(&func3);
    auto f4 = bp3::def(std::function<int(Example&,double)>(func4));

    f1.call();
    f2.call();
    f3.call();
    f4.call();

    Py_RETURN_NONE;
}


static PyMethodDef methods[] = {
    {"run", run, METH_NOARGS, "run the example"},
    {0, 0, 0, 0}
};

PyMODINIT_FUNC initexample() {
    PyObject * m = Py_InitModule("example", methods);
}
