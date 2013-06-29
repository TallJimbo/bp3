#include "bp3/module.hpp"
#include "bp3/conversion/registration.hpp"

#include <cstring>
#include <map>

namespace bp3 {

namespace {

struct compare_type_info {
    bool operator()(bp3::type_info const & a, bp3::type_info const & b) const {
        return std::strcmp(a.name(), b.name()) < 0;
    }
};

typedef std::map<bp3::type_info,std::shared_ptr<conversion::registration>,compare_type_info> registry_map;

registry_map & extract(py_ptr const & pyregistry) {
    return *reinterpret_cast<registry_map*>(PyCapsule_GetPointer(pyregistry.get(), "bp3.registry"));
}

void destroy_registry_map(PyObject * capsule) {
    registry_map * p = reinterpret_cast<registry_map*>(PyCapsule_GetPointer(capsule, "bp3.registry"));
    delete p;
}

} // anonymous

void module::add(std::string const & name, py_ptr const & ptr) {
    if (PyModule_AddObject(_pymod.get(), name.c_str(), ptr.incref()) < 0) {
        throw_error_already_set();
    }
}

std::shared_ptr<conversion::registration> module::lookup(bp3::type_info const & t) const {
    registry_map & registry = extract(_pyregistry);
    registry_map::const_iterator iter = registry.find(t);
    std::shared_ptr<conversion::registration> result;
    if (iter != registry.end()) result = iter->second;
    return result;
}

void module::register_from_python(
    bp3::type_info const & t, bool is_lvalue,
    conversion::from_python_check_func check,
    conversion::from_python_convert_func convert,
    conversion::from_python_postcall_func postcall,
    conversion::from_python_cleanup_func cleanup
) {
    registry_map & registry = extract(_pyregistry);
    std::shared_ptr<conversion::registration> & reg = registry[t];
    if (!reg) {
        reg = std::make_shared<conversion::registration>();
    }
    reg->from_python.front()->push_front(
        conversion::from_python_funcs{is_lvalue, check, convert, postcall, cleanup}
    );
}

module::module(py_ptr const & pymod) :
    _pymod(pymod),
    _pyregistry(py_ptr::steal(PyCapsule_New(new registry_map, "bp3.registry", destroy_registry_map)))
{
    PyModule_AddObject(pymod.get(), "registry", _pyregistry.incref());
    // TODO: add default converters
}

module::~module() {}

} // namespace bp3
