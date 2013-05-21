#include "bp3/builtin.hpp"

#include <vector>

#include "clang-c/Index.h"

namespace bp3 {

static PyObject * run(PyObject * self, PyObject * args) {
    Py_ssize_t nargs = PySequence_Size(args);
    std::vector<std::string> str_args(nargs);
    std::vector<char const *> cstr_args(nargs);
    try {
        for (Py_ssize_t i = 0; i < nargs; ++i) {
            py_ptr item = py_ptr::steal(PySequence_GetItem(args, i));
            str_args[i] = builtin::str(item);
            cstr_args[i] = str_args[i].c_str();
        }
    } catch (builtin::Exception & err) {
        return err.release();
    }
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit tu = clang_parseTranslationUnit(index, 0, &cstr_args.front(), nargs, nullptr, 0,
                                                      CXTranslationUnit_None);
    

    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(index);
    Py_RETURN_NONE;
}

} // namspace bp3

static PyMethodDef methods[] = {
    {"run", (PyCFunction)&bp3::run, METH_VARARGS, nullptr},
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
