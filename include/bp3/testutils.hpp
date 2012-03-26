#ifndef BP3_testutils_hpp_INCLUDED
#define BP3_testutils_hpp_INCLUDED

#include "bp3/py_ptr.hpp"

#include <string>
#include <iostream>

#ifdef __GNUC__
#define BP3_TEST_FUNCNAME __PRETTY_FUNCTION__
#else
#define BP3_TEST_FUNCNAME __func__
#endif

#define BP3_TEST(x) if (!(x)) fail(__LINE__, BP3_TEST_FUNCNAME)

#define BP3_TEST_PYTHON(code) if (!eval(code)) fail(__LINE__, BP3_TEST_FUNCNAME)

namespace bp3 {

template <typename Derived>
class test_suite {
public:

    typedef void (Derived::*testfunc)();

    explicit test_suite(std::string const & name) : _status(true), _name(name) {
#if PY_MAJOR_VERSION == 2
        Py_Initialize();
        Py_InitModule(_name.c_str(), Derived::methods);
#else
        test_module.m_name = _name.c_str();
        test_module.m_methods = Derived::methods;
        PyImport_AppendInittab(_name.c_str(), &PyInit_test_module);
        Py_Initialize();
#endif
    }

    ~test_suite() { Py_Finalize(); }

    void fail(int line, char const * func) {
        _status = false;
        std::cerr << "Failure in " << func << ": " << line << std::endl;
    }

    bool eval(char const * code) {
        bp3::py_ptr main = bp3::py_ptr::borrow(PyImport_AddModule("__main__"));
        bp3::py_ptr module = bp3::py_ptr::steal(PyImport_ImportModule(_name.c_str()));
        bp3::py_ptr globals = bp3::py_ptr::steal(PyObject_GetAttrString(main.get(), "__dict__"));
        bp3::py_ptr results = bp3::py_ptr::steal(PyDict_New());
        PyDict_SetItemString(globals.get(), "results", results.get());
        PyDict_SetItemString(globals.get(), _name.c_str(), module.get());
        bp3::py_ptr locals = bp3::py_ptr::steal(PyDict_New());
        bp3::py_ptr status = bp3::py_ptr::steal(
            PyRun_String(code, Py_file_input, globals.get(), locals.get())
        );
        bool ret = true;
        if (!status) {
            std::cerr << "Exception in embedded Python code:" << std::endl;
            PyErr_Print();
            ret = false;
        }
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(results.get(), &pos, &key, &value)) {
            if (PyObject_Not(value)) {
                bp3::py_ptr s = bp3::py_ptr::steal(PyObject_Bytes(key));
                std::cerr << "Embedded Python test failure for key '" 
                          << PyBytes_AS_STRING(s.get()) << "'" << std::endl;
                ret = false;
            }
        }
        return ret;
    }

    void setUp() {}

    void tearDown() {}

    static bool run(std::initializer_list<testfunc> funcs) {
        Derived suite;
        for (testfunc func : funcs) {
            suite.setUp();
            (suite.*func)();
            suite.tearDown();
        }
        return suite._status;
    }

private:

    static PyMethodDef methods[];

#if PY_MAJOR_VERSION != 2
    static PyModuleDef test_module;
    static PyObject * PyInit_test_module() {
        return PyModule_Create(&test_module);
    }
#endif

    bool _status;
    std::string _name;
};

template <typename Derived>
PyMethodDef test_suite<Derived>::methods[] = {
    {0, 0, 0, 0}
};

#if PY_MAJOR_VERSION != 2
template <typename Derived>
PyModuleDef test_suite<Derived>::test_module = { PyModuleDef_HEAD_INIT };
#endif

} // namespace bp3

#endif // !BP3_testutils_hpp_INCLUDED
