#include "bp3/Module.hpp"
#include "bp3/Callable.hpp"
#include "bp3/Registry.hpp"
#include "bp3/builtin/numbers.hpp"
#include "bp3/Debug.hpp"

#include <iostream>
#include <string>

namespace bp3 {

namespace {

static PyMethodDef methods[] = {
    {nullptr, nullptr, 0, nullptr}
};

} // anonymous

void initNumericTypes(Module & module);
void initExceptions(Module & module);

class LibraryAccess {
public:

    static void initRegistryType(PyObject * m) {
        if (PyModule_AddObject(m, "Registry", bp3::Registry::initType().incref()) < 0) {
            throw_error_already_set();
        }
    }
    static void initOtherTypes(Module & module) {
        module.add("Callable", bp3::Callable::initType());
        initNumericTypes(module);
        // TODO: add more default converters
        initExceptions(module);
    }

};

} // namespace bp3

#if PY_MAJOR_VERSION == 2
PyMODINIT_FUNC
init_bp3() {
    PyObject * m = Py_InitModule("_bp3", bp3::methods);
    if (!m) return;
    try {
        bp3::LibraryAccess::initRegistryType(m);
        bp3::Module mod(bp3::PyPtr::borrow(m));
        bp3::LibraryAccess::initOtherTypes(mod);
    } catch (bp3::builtin::BaseException & err) {
        err.release();
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "unknown C++ exception");
    }
}
#else // Python 3
static PyModuleDef _bp3_lib_module_def = {
    PyModuleDef_HEAD_INIT,
    "_bp3",
    nullptr,
    -1,
    bp3::methods
};
PyMODINIT_FUNC
PyInit__bp3() {
    PyObject * m = PyModule_Create(&_bp3_lib_module_def);
    if (!m) return m;
    try {
        bp3::LibraryAccess::initRegistryType(m);
        bp3::Module mod(bp3::PyPtr::borrow(m));
        bp3::LibraryAccess::initOtherTypes(mod);
    } catch (bp3::builtin::BaseException & err) {
        err.release();
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "unknown C++ exception");
    }
    return m;
}
#endif
