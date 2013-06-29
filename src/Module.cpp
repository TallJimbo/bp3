#include "bp3/Module.hpp"
#include "bp3/Registration.hpp"

#include <cstring>
#include <map>

namespace bp3 {

namespace {

struct compareTypeInfo {
    bool operator()(bp3::TypeInfo const & a, bp3::TypeInfo const & b) const {
        return std::strcmp(a.name(), b.name()) < 0;
    }
};

typedef std::map<bp3::TypeInfo,std::shared_ptr<Registration>,compareTypeInfo> RegistryMap;

RegistryMap & extract(PyPtr const & pyregistry) {
    return *reinterpret_cast<RegistryMap*>(PyCapsule_GetPointer(pyregistry.get(), "bp3.registry"));
}

void destroyRegistryMap(PyObject * capsule) {
    RegistryMap * p = reinterpret_cast<RegistryMap*>(PyCapsule_GetPointer(capsule, "bp3.registry"));
    delete p;
}

} // anonymous

void Module::add(std::string const & name, PyPtr const & ptr) {
    if (PyModule_AddObject(_pymod.get(), name.c_str(), ptr.incref()) < 0) {
        throw_error_already_set();
    }
}

std::shared_ptr<Registration> Module::lookup(bp3::TypeInfo const & t) const {
    RegistryMap & registry = extract(_pyregistry);
    RegistryMap::const_iterator iter = registry.find(t);
    std::shared_ptr<Registration> result;
    if (iter != registry.end()) result = iter->second;
    return result;
}

void Module::registerFromPython(
    TypeInfo const & t, bool is_lvalue,
    FromPythonCheckFunc check,
    FromPythonConvertFunc convert,
    FromPythonPostcallFunc postcall,
    FromPythonCleanupFunc cleanup
) {
    RegistryMap & registry = extract(_pyregistry);
    std::shared_ptr<Registration> & reg = registry[t];
    if (!reg) {
        reg = std::make_shared<Registration>();
    }
    reg->from_python.front()->push_front(
        FromPythonFuncs{is_lvalue, check, convert, postcall, cleanup}
    );
}

Module::Module(PyPtr const & pymod) :
    _pymod(pymod),
    _pyregistry(PyPtr::steal(PyCapsule_New(new RegistryMap, "bp3.registry", destroyRegistryMap)))
{
    PyModule_AddObject(pymod.get(), "registry", _pyregistry.incref());
    // TODO: add default converters
}

Module::~Module() {}

} // namespace bp3
