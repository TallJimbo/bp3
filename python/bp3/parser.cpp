#include "bp3/builtin.hpp"

namespace bp3 {



} // namspace bp3

static PyMethodDef methods[] = {
    {nullptr, nullptr, 0, nullptr}
};


#if PY_MAJOR_VERSION == 2

PyMODINIT_FUNC
initparser() {
    PyObject * m = Py_InitModule("parser", methods);
    if (!m) return;
}

#else

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "parser",
    nullptr,
    -1,
    methods
};

PyMODINIT_FUNC
PyInit_from_python_mod() {
    PyObject * m = PyModule_Create(&module);
    if (!m) return nullptr;
    return m;
}

#endif
