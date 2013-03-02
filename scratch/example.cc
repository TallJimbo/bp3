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

    auto f1 = bp3::def("f1", &Example::func1, {"x"});
    auto f2 = bp3::def("f2", &Example::func2, {"x"});
    auto f3 = bp3::def("f3", &func3, {"x"});
    auto f4 = bp3::def("f4", std::function<int(Example&,double)>(func4), {"x"});

    bp3::py_ptr args = bp3::py_ptr::steal(PyTuple_New(1));
    PyTuple_SET_ITEM(args.get(), 0, PyFloat_FromDouble(3.2));
    bp3::py_ptr kwargs = bp3::py_ptr::steal(PyDict_New());

    f1.call(args, kwargs);
    f2.call(args, kwargs);
    f3.call(args, kwargs);
    f4.call(args, kwargs);

    Py_RETURN_NONE;
}


static PyMethodDef methods[] = {
    {"run", run, METH_NOARGS, "run the example"},
    {0, 0, 0, 0}
};

#if PY_MAJOR_VERSION != 2
static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "example",
    0,
    0,
    methods
};
#endif

PyMODINIT_FUNC initexample() {
#if PY_MAJOR_VERSION == 2
    Py_InitModule("example", methods);
#else
    return PyModule_Create(&module);
#endif
}
