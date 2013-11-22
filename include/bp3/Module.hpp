#ifndef BP3_Module_hpp_INCLUDED
#define BP3_Module_hpp_INCLUDED

#include "bp3/TypeInfo.hpp"
#include "bp3/FromPythonBase.hpp"
#include "bp3/builtin/exceptions.hpp"
#include "bp3/Registry.hpp"

#include <string>
#include <memory>

#if PY_MAJOR_VERSION == 2
#define BP3_MODULE(name, callback, methods)                             \
    PyMODINIT_FUNC                                                      \
    init ## name() {                                                    \
        try {                                                           \
            bp3::Module bp3_module("bp3");                              \
            PyObject * m = Py_InitModule(#name, methods);               \
            if (!m) return;                                             \
            bp3::Module module(bp3::PyPtr::borrow(m));                  \
            module.getRegistry().import(bp3_module.getRegistry());      \
            callback(module);                                              \
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
        try {                                                           \
            bp3::Module bp3_module("bp3");                              \
            PyObject * m = PyModule_Create(&_bp3_ ## name ## _module_def); \
            if (!m) return m;                                           \
            bp3::Module module(bp3::PyPtr::borrow(m));                  \
            module.getRegistry().import(bp3_module.getRegistry());      \
            callback(module);                                           \
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

class Module {
public:

    // Create a Module from a newly-created module PyObject, and install a new Registry in it.
    explicit Module(PyPtr const & ptr);

    // Import an existing bp3 Module
    explicit Module(std::string const & name);

    // Import another bp3 module, and import its Registry into this.
    // Use another method (TODO) to import a non-bp3 module, or to avoid importing the Registry.
    Module import(std::string const & name) const;

    void add(std::string const & name, PyPtr const & ptr) const;

    Registry const & getRegistry() const { return _registry; }

    PyPtr const & ptr() const { return _ptr; }

private:

    PyPtr _ptr;
    Registry _registry;
};

} // namespace bp3

#endif // !BP3_Module_hpp_INCLUDED
