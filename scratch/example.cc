#include "bp3/py_ptr.hpp"
#include "bp3/callable.hpp"

class Example {
public:

    int func1(double) { return 1; }
    int func2(double) const { return 2; }

    static bp3::py_ptr make() {
        Example * x = new Example();
        return bp3::py_ptr::steal(PyCapsule_New(x, "Example", &destroy));
    }

    static int check(bp3::py_ptr const & ptr, bp3::converter_data &) {
        if (PyCapsule_IsValid(ptr.get(), "Example")) return 0;
        return -1;
    }

    static void * convert(bp3::py_ptr const & ptr, bp3::converter_data &) {
        return PyCapsule_GetPointer(ptr.get(), "Example");
    }

    explicit Example() {}

private:
    static void destroy(PyObject * capsule) {
        Example * p = reinterpret_cast<Example*>(PyCapsule_GetPointer(capsule, "Example"));
        delete p;
    }
};

int func3(Example &, double) { return 3; }

class FuncN {
public:

    int operator()(Example &, double) const { return n; }

    int n;

};

PyObject * run(PyObject *, PyObject *) {
    try {
        FuncN func4 = { 4 };

        bp3::module mod;

        mod.register_from_python(
            bp3::type_id<Example>(), true,
            Example::check, Example::convert
        );

        auto f1 = bp3::callable(mod, "f1", &Example::func1, {"x"});
        auto f2 = bp3::callable(mod, "f2", &Example::func2, {"x"});
        auto f3 = bp3::callable(mod, "f3", &func3, {"x"});
        auto f4 = bp3::callable(mod, "f4", std::function<int(Example&,double)>(func4), {"x"});

        bp3::py_ptr args = bp3::py_ptr::steal(PyTuple_New(2));
        PyTuple_SET_ITEM(args.get(), 0, Example::make().release());
        PyTuple_SET_ITEM(args.get(), 1, PyFloat_FromDouble(3.2));
        bp3::py_ptr kwargs = bp3::py_ptr::steal(PyDict_New());

        f1.call(args, kwargs);
        f2.call(args, kwargs);
        f3.call(args, kwargs);
        f4.call(args, kwargs);

    } catch (bp3::builtin::Exception & err) {
        err.release();
        return nullptr;
    }

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
