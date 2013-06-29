#ifndef BP3_module_hpp_INCLUDED
#define BP3_module_hpp_INCLUDED

#include "bp3/TypeInfo.hpp"
#include "bp3/conversion/from_python_base.hpp"
#include "bp3/builtin/exceptions.hpp"

#include <string>
#include <memory>

#if PY_MAJOR_VERSION == 2
#define BP3_MODULE(name, callback, methods)                             \
    PyMODINIT_FUNC                                                      \
    init ## name() {                                                    \
        PyObject * m = Py_InitModule(#name, methods);                   \
        if (!m) return;                                                 \
        bp3::module mod(bp3::PyPtr::borrow(m));                        \
        try {                                                           \
            callback(mod);                                              \
        } catch (bp3::builtin::BaseException & err) {                   \
            err.release();                                              \
        } catch (...) {                                                 \
            PyErr_SetString(PyExc_RuntimeError, "unknown C++ exception"); \
        }                                                               \
    }
#else // Python 3
#define BP3_MODULE(name, callback, methods)                             \
    static PyModuleDef _bp3_module_def = {                              \
        PyModuleDef_HEAD_INIT,                                          \
        #name,                                                          \
        nullptr,                                                        \
        -1,                                                             \
        methods                                                         \
    };                                                                  \
    PyMODINIT_FUNC                                                      \
    PyInit_ ## name() {                                                 \
        PyObject * m = PyModule_Create(&_bp3_module_def);               \
        if (!m) return m;                                               \
        bp3::module mod(bp3::PyPtr::borrow(m));                        \
        try {                                                           \
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

namespace conversion {

class registration;

} // namespace conversion

class module {
public:

    module(PyPtr const & pymod);

    void add(std::string const & name, PyPtr const & ptr);

    std::shared_ptr<conversion::registration> lookup(bp3::TypeInfo const & t) const;

    void register_from_python(
        bp3::TypeInfo const & t, bool is_lvalue,
        conversion::from_python_check_func check,
        conversion::from_python_convert_func convert,
        conversion::from_python_postcall_func postcall,
        conversion::from_python_cleanup_func cleanup=nullptr
    );

    void register_from_python(
        bp3::TypeInfo const & t, bool is_lvalue,
        conversion::from_python_check_func check,
        conversion::from_python_convert_func convert,
        conversion::from_python_cleanup_func cleanup=nullptr
    ) {
        register_from_python(t, is_lvalue, check, convert, nullptr, cleanup);
    }

    ~module();

private:

    PyPtr _pymod;
    PyPtr _pyregistry;
};

} // namespace bp3

#endif // !BP3_module_hpp_INCLUDED
