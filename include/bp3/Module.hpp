#ifndef BP3_Module_hpp_INCLUDED
#define BP3_Module_hpp_INCLUDED

#include "bp3/TypeInfo.hpp"
#include "bp3/FromPythonBase.hpp"
#include "bp3/builtin/exceptions.hpp"

#include <string>
#include <memory>

#if PY_MAJOR_VERSION == 2
#define BP3_MODULE(name, callback, methods)                             \
    PyMODINIT_FUNC                                                      \
    init ## name() {                                                    \
        PyObject * m = Py_InitModule(#name, methods);                   \
        if (!m) return;                                                 \
        try {                                                           \
            bp3::Module mod(#name, bp3::PyPtr::borrow(m));              \
            callback(mod);                                              \
        } catch (bp3::builtin::BaseException & err) {                   \
            err.release();                                              \
        } catch (...) {                                                 \
            PyErr_SetString(PyExc_RuntimeError, "unknown C++ exception"); \
        }                                                               \
    }
#else // Python 3
#define BP3_MODULE(name, callback, methods)                             \
    static PyModuleDef _bp3_ ## name ## _module_def = {                 \
        PyModuleDef_HEAD_INIT,                                          \
        #name,                                                          \
        nullptr,                                                        \
        -1,                                                             \
        methods                                                         \
    };                                                                  \
    PyMODINIT_FUNC                                                      \
    PyInit_ ## name() {                                                 \
        PyObject * m = PyModule_Create(&_bp3_ ## name ## _module_def);  \
        if (!m) return m;                                               \
        try {                                                           \
            bp3::Module mod(#name, bp3::PyPtr::borrow(m));              \
            callback(mod);                                              \
        } catch (bp3::builtin::BaseException & err) {                   \
            err.release();                                              \
        } catch (...) {                                                 \
            PyErr_SetString(PyExc_RuntimeError, "unknown C++ exception"); \
        }                                                               \
        return m;                                                       \
    }
#endif

namespace bp3 {

class Registration;
class Registry;

class Module {
public:

    Module(char const * name, PyPtr const & pymodule);

    void add(std::string const & name, PyPtr const & ptr);

    std::shared_ptr<Registration> lookup(TypeInfo const & t) const;

    void registerFromPython(
        TypeInfo const & t, bool is_lvalue,
        FromPythonCheckFunc check,
        FromPythonConvertFunc convert,
        FromPythonPostcallFunc postcall,
        FromPythonCleanupFunc cleanup=nullptr
    );

    void registerFromPython(
        TypeInfo const & t, bool is_lvalue,
        FromPythonCheckFunc check,
        FromPythonConvertFunc convert,
        FromPythonCleanupFunc cleanup=nullptr
    ) {
        registerFromPython(t, is_lvalue, check, convert, nullptr, cleanup);
    }

    ~Module();

private:

    PyPtr _pymodule;
    PyPtr _pyregistry;
    Registry * _registry;
};

} // namespace bp3

#endif // !BP3_Module_hpp_INCLUDED