#include "bp3/module.hpp"
#include "bp3/conversion/from_python.hpp"

#include <string>

/*
 *  Unit test for from_python converters.
 *
 *  We wrap the example classes using Python's built-in Capsule objects
 *  so as not to get conversion mixed up with wrapping at this stage.
 */

namespace {

class Example1 {
public:

    static bp3::py_ptr make(std::string const & value) {
        Example1 * x = new Example1(value);
        return bp3::py_ptr::steal(PyCapsule_New(x, "Example1", &destroy));
    }

    static int check1(bp3::py_ptr const & ptr, bp3::converter_data &) {
        if (PyCapsule_IsValid(ptr.get(), "Example1")) return 0;
        return -1;
    }

    static void * convert1(bp3::py_ptr const & ptr, bp3::converter_data &) {
        return PyCapsule_GetPointer(ptr.get(), "Example1");
    }

    explicit Example1(std::string const & x) : value(x) {}

    std::string value;

private:
    static void destroy(PyObject * capsule) {
        Example1 * p = reinterpret_cast<Example1*>(PyCapsule_GetPointer(capsule, "Example1"));
        delete p;
    }
};

static bp3::module mod;

template <typename T>
static PyObject * check_rv(PyObject * self, PyObject * arg) {
    bp3::py_ptr py1 = bp3::py_ptr::borrow(arg);
    bp3::conversion::from_python<T> converter(mod, py1);
    if (!converter.is_convertible()) {
        Py_RETURN_FALSE;
    }
    T ex1 = converter.convert();
    if (ex1.value != "ex1") {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

template <typename T>
static PyObject * check_ptr(PyObject * self, PyObject * arg) {
    bp3::py_ptr py1 = bp3::py_ptr::borrow(arg);
    bp3::conversion::from_python<T> converter(mod, py1);
    if (!converter.is_convertible()) {
        Py_RETURN_FALSE;
    }
    T ex1 = converter.convert();
    if (ex1->value != "ex1") {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

} // namespace

static PyMethodDef methods[] = {
    {"check_val", &check_rv<Example1>, METH_O, nullptr},
    {"check_cval", &check_rv<Example1 const>, METH_O, nullptr},
    {"check_ref", &check_rv<Example1 &>, METH_O, nullptr},
    {"check_cref", &check_rv<Example1 const &>, METH_O, nullptr},
    {"check_ptr", &check_ptr<Example1*>, METH_O, nullptr},
    {"check_cptr", &check_ptr<Example1 const *>, METH_O, nullptr},
    {"check_ptrc", &check_ptr<Example1 * const>, METH_O, nullptr},
    {"check_cptrc", &check_ptr<Example1 const * const>, METH_O, nullptr},
    {nullptr, nullptr, 0, nullptr}
};

#if PY_MAJOR_VERSION == 2

PyMODINIT_FUNC
initfrom_python_mod() {
    PyObject * m = Py_InitModule("from_python_mod", methods);

    if (!m) return;

    mod.register_from_python(
        bp3::type_id<Example1>(), true, &Example1::check1, &Example1::convert1
    );
    PyModule_AddObject(m, "py1", Example1::make("ex1").release());
}

#else

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "from_python_mod",
    nullptr,
    -1,
    methods
};

PyMODINIT_FUNC
PyInit_from_python_mod() {
    PyObject * m = PyModule_Create(&module);
    if (!m) return m;
    mod.register_from_python(
        bp3::type_id<Example1>(), true, &Example1::check1, &Example1::convert1
    );
    PyModule_AddObject(m, "py1", Example1::make("ex1").release());
    return m;
}

#endif
