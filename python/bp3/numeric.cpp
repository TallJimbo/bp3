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

    static PyPtr toPython(T * value) {
        return PyPtr::steal(PyFloat_FromDouble(*value));
    }

    static void declare(Registry const & registry) {
        registry.registerFromPython(makeTypeInfo<T>(), false, check, convert, cleanup);
        registry.registerMoveToPython(makeTypeInfo<T>(), (MoveToPythonFunc)toPython);
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
        if (std::is_signed<T>::value) {
            data.scratch2 = new T(PyLong_AsLongLong(p));
        } else {
            data.scratch2 = new T(PyLong_AsUnsignedLongLong(p));
        }
        return data.scratch2;
    }

    static void cleanup(ConverterData & data) {
        Log<15>::get() << "IntConverters::cleanup: data at " << &data << "\n";
        Py_XDECREF((PyObject*)data.scratch1);
        delete reinterpret_cast<T*>(data.scratch2);
    }

    static PyPtr toPython(T * value) {
#if PY_MAJOR_VERSION == 2
        if (sizeof(T) <= sizeof(std::size_t)) {
            if (std::is_signed<T>::value) {
                return PyPtr::steal(PyInt_FromSsize_t(*value));
            } else {
                return PyPtr::steal(PyInt_FromSize_t(*value));
            }
        }
#endif
        if (std::is_signed<T>::value) {
            return PyPtr::steal(PyLong_FromLongLong(*value));
        } else {
            return PyPtr::steal(PyLong_FromUnsignedLongLong(*value));
        }
    }

    static void declare(Registry const & registry) {
        registry.registerFromPython(makeTypeInfo<T>(), false, check, convert, cleanup);
        registry.registerMoveToPython(makeTypeInfo<T>(), (MoveToPythonFunc)toPython);
    }

};

} // anonymous

void initNumericTypes(Module & module) {
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
}

} // namespace bp3
