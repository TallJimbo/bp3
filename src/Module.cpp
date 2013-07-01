#include "bp3/Module.hpp"
#include "bp3/Registration.hpp"
#include "bp3/builtin/numbers.hpp"

#include <cstring>
#include <map>

namespace bp3 {

namespace {

template <typename T>
struct FloatConverters {

    static int check(PyPtr const & py, ConverterData & data) {
        if (PyFloat_CheckExact(py.get())) {
            return 0;
        }
        if (PyFloat_Check(py.get())) {
            return 1;
        }
        PyPtr f = PyPtr::steal(PyNumber_Float(py.get()));
        if (!f) {
            PyErr_Clear();
            return -1;
        }
        data.scratch1 = f.release();
        return 2;
    }

    static void * convert(PyPtr const & py, ConverterData & data) {
        PyObject * p = (data.scratch1) ? (PyObject*)data.scratch1 : py.get();
        data.scratch2 = new T(PyFloat_AsDouble(p));
        return data.scratch2;
    }

    static void cleanup(ConverterData & data) {
        Py_XDECREF((PyObject*)data.scratch1);
        delete reinterpret_cast<T*>(data.scratch2);
    }

    static void declare(Module & module) {
        module.registerFromPython(makeTypeInfo<T>(), false, check, convert, cleanup);
    }

};

template <typename T>
struct IntConverters {

    static int check(PyPtr const & py, ConverterData & data) {
#if PY_MAJOR_VERSION == 2
        if (PyInt_CheckExact(py.get())) {
            return 0;
        }
#endif
        if (PyLong_CheckExact(py.get())) {
            return 0;
        }
#if PY_MAJOR_VERSION == 2
        if (PyInt_Check(py.get())) {
            return 1;
        }
#endif
        if (PyLong_Check(py.get())) {
            return 0;
        }
        PyPtr f = PyPtr::steal(PyNumber_Long(py.get()));
        if (!f) {
            PyErr_Clear();
            return -1;
        }
        data.scratch1 = f.release();
        return 2;
    }

    static void * convert(PyPtr const & py, ConverterData & data) {
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
        Py_XDECREF((PyObject*)data.scratch1);
        delete reinterpret_cast<T*>(data.scratch2);
    }

    static void declare(Module & module) {
        module.registerFromPython(makeTypeInfo<T>(), false, check, convert, cleanup);
    }

};



struct compareTypeInfo {
    bool operator()(bp3::TypeInfo const & a, bp3::TypeInfo const & b) const {
        return std::strcmp(a.name(), b.name()) < 0;
    }
};

typedef std::map<bp3::TypeInfo,std::shared_ptr<Registration>,compareTypeInfo> RegistryMap;

RegistryMap & extract(PyPtr const & pyregistry) {
    return *reinterpret_cast<RegistryMap*>(PyCapsule_GetPointer(pyregistry.get(), "bp3.registry"));
}

} // anonymous

class Registry {
public:

    std::string const name;
    RegistryMap map;

    static Registry * make(char const * module_name, PyPtr const & pymodule, PyPtr & pyregistry) {
        Registry * p = new Registry(module_name);
        pyregistry = PyPtr::steal(PyCapsule_New(p, p->name.c_str(), destroy));
        if (PyModule_AddObject(pymodule.get(), "registry", pyregistry.release()) < 0) {
            throw_error_already_set();
        }
        return p;
    }

private:

    explicit Registry(char const * module_name) : name(std::string(module_name) + ".registry") {}

    static void destroy(PyObject * capsule) {
        char const * name = PyCapsule_GetName(capsule);
        Registry * p = reinterpret_cast<Registry*>(PyCapsule_GetPointer(capsule, name));
        delete p;
    }
};

void Module::add(std::string const & name, PyPtr const & ptr) {
    if (PyModule_AddObject(_pymodule.get(), name.c_str(), ptr.incref()) < 0) {
        throw_error_already_set();
    }
}

std::shared_ptr<Registration> Module::lookup(bp3::TypeInfo const & t) const {
    RegistryMap::const_iterator iter = _registry->map.find(t);
    std::shared_ptr<Registration> result;
    if (iter != _registry->map.end()) result = iter->second;
    return result;
}

void Module::registerFromPython(
    TypeInfo const & t, bool is_lvalue,
    FromPythonCheckFunc check,
    FromPythonConvertFunc convert,
    FromPythonPostcallFunc postcall,
    FromPythonCleanupFunc cleanup
) {
    std::shared_ptr<Registration> & reg = _registry->map[t];
    if (!reg) {
        reg = std::make_shared<Registration>();
    }
    reg->from_python.front()->push_front(
        FromPythonFuncs{is_lvalue, check, convert, postcall, cleanup}
    );
}

Module::Module(char const * name, PyPtr const & pymodule) :
    _pymodule(pymodule), _registry(Registry::make(name, _pymodule, _pyregistry))
{
    FloatConverters<float>::declare(*this);
    FloatConverters<double>::declare(*this);
    IntConverters<short>::declare(*this);
    IntConverters<unsigned short>::declare(*this);
    IntConverters<int>::declare(*this);
    IntConverters<unsigned int>::declare(*this);
    IntConverters<long>::declare(*this);
    IntConverters<unsigned long>::declare(*this);
    IntConverters<long long>::declare(*this);
    IntConverters<unsigned long long>::declare(*this);
    // TODO: add default converters
}

Module::~Module() {}

} // namespace bp3