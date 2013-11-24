#include "bp3/Module.hpp"
#include "bp3/Callable.hpp"
#include "bp3/Registry.hpp"
#include "bp3/builtin/numbers.hpp"
#include "bp3/Debug.hpp"

#include <iostream>
#include <string>

namespace bp3 {

namespace {

template <typename T>
struct FloatConverters {

    static int check(PyPtr const & py, ConverterData & data) {
        Log<15>::get() << "FloatConverters::check: data at " << &data << "\n";
        if (PyFloat_CheckExact(py.get())) {
            Log<15>::get() << "FloatConverters::check: exact match\n";
            return 0;
        }
        if (PyFloat_Check(py.get())) {
            Log<15>::get() << "FloatConverters::check: subclass match\n";
            return 1;
        }
        PyPtr f = PyPtr::steal(PyNumber_Float(py.get()));
        if (!f) {
            Log<15>::get() << "FloatConverters::check: no match\n";
            PyErr_Clear();
            return -1;
        }
        Log<15>::get() << "FloatConverters::check: inexact match; setting scratch1\n";
        data.scratch1 = f.release();
        return 2;
    }

    static void * convert(PyPtr const & py, ConverterData & data) {
        Log<15>::get() << "FloatConverters::convert: data at " << &data << "\n";
        PyObject * p = (data.scratch1) ? (PyObject*)data.scratch1 : py.get();
        data.scratch2 = new T(PyFloat_AsDouble(p));
        return data.scratch2;
    }

    static void cleanup(ConverterData & data) {
        Log<15>::get() << "FloatConverters::cleanup: data at " << &data << "\n";
        Py_XDECREF((PyObject*)data.scratch1);
        delete reinterpret_cast<T*>(data.scratch2);
    }

    static void declare(Registry const & registry) {
        registry.registerFromPython(makeTypeInfo<T>(), false, check, convert, cleanup);
    }

};

template <typename T>
struct IntConverters {

    static int check(PyPtr const & py, ConverterData & data) {
        Log<15>::get() << "IntConverters::check: data at " << &data << "\n";
#if PY_MAJOR_VERSION == 2
        if (PyInt_CheckExact(py.get())) {
            Log<15>::get() << "IntConverters::check: exact match\n";
            return 0;
        }
#endif
        if (PyLong_CheckExact(py.get())) {
            Log<15>::get() << "IntConverters::check: exact match\n";
            return 0;
        }
#if PY_MAJOR_VERSION == 2
        if (PyInt_Check(py.get())) {
            Log<15>::get() << "IntConverters::check: subclass match\n";
            return 1;
        }
#endif
        if (PyLong_Check(py.get())) {
            Log<15>::get() << "IntConverters::check: subclass match\n";
            return 0;
        }
        PyPtr f = PyPtr::steal(PyNumber_Long(py.get()));
        if (!f) {
            Log<15>::get() << "IntConverters::check: no match\n";
            PyErr_Clear();
            return -1;
        }
        Log<15>::get() << "IntConverters::check: inexact match; setting scratch1\n";
        data.scratch1 = f.release();
        return 2;
    }

    static void * convert(PyPtr const & py, ConverterData & data) {
        Log<15>::get() << "IntConverters::convert: data at " << &data << "\n";
        PyObject * p = (data.scratch1) ? (PyObject*)data.scratch1 : py.get();
        if (data.scratch1) {
            if (std::is_signed<T>::value) {
                data.scratch2 = new T(PyLong_AsLongLong(p));
            } else {
                data.scratch2 = new T(PyLong_AsUnsignedLongLong(p));
            }
        }
        return data.scratch2;
    }

    static void cleanup(ConverterData & data) {
        Log<15>::get() << "IntConverters::cleanup: data at " << &data << "\n";
        Py_XDECREF((PyObject*)data.scratch1);
        delete reinterpret_cast<T*>(data.scratch2);
    }

    static void declare(Registry const & registry) {
        registry.registerFromPython(makeTypeInfo<T>(), false, check, convert, cleanup);
    }

};

static PyMethodDef methods[] = {
    {nullptr, nullptr, 0, nullptr}
};

} // anonymous

class LibraryAccess {
public:

    static void initRegistryType(PyObject * m) {
        if (PyModule_AddObject(m, "Registry", bp3::Registry::initType().incref()) < 0) {
            throw_error_already_set();
        }
    }

    static void initOtherTypes(Module & module) {
        module.add("Callable", bp3::Callable::initType());
        FloatConverters<float>::declare(module.getRegistry());
        FloatConverters<double>::declare(module.getRegistry());
        IntConverters<short>::declare(module.getRegistry());
        IntConverters<unsigned short>::declare(module.getRegistry());
        IntConverters<int>::declare(module.getRegistry());
        IntConverters<unsigned int>::declare(module.getRegistry());
        IntConverters<long>::declare(module.getRegistry());
        IntConverters<unsigned long>::declare(module.getRegistry());
        IntConverters<long long>::declare(module.getRegistry());
        IntConverters<unsigned long long>::declare(module.getRegistry());
        // TODO: add more default converters
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
