#include "bp3/Callable.hpp"
#include "bp3/Registration.hpp"
#include "bp3/Module.hpp"

#include <sstream>

namespace bp3 {

namespace {

struct RegistryImpl : public ::PyObject {

    static PyTypeObject type;

    RegistryImpl() {
        PyObject_Init(this, &type);
    }

    static void destroy(::PyObject * self);

    Registration::Map map;
};

Registration::Map & getMap(Registry const * self) {
    return static_cast<RegistryImpl*>(self->ptr().get())->map;
}

void RegistryImpl::destroy(::PyObject * self) {
    delete static_cast<RegistryImpl*>(self);
}

PyTypeObject RegistryImpl::type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    const_cast<char*>("bp3.Registry"),  /*tp_name*/
    sizeof(RegistryImpl),      /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)&RegistryImpl::destroy,    /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "bp3 type conversion registry",           /* tp_doc */
    0,		                   /* tp_traverse */
    0,		                   /* tp_clear */
    0,		                   /* tp_richcompare */
    0,		                   /* tp_weaklistoffset */
    0,		                   /* tp_iter */
    0,		                   /* tp_iternext */
    0,                         /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    0,                         /* tp_new */
    0,                         /* tp_free */
};

static char const * REGISTRY_ATTRIBUTE_NAME = "__bp3_registry__";

} // anonymous

Registry::Registry(Module const & module, bool install_if_missing) :
    _ptr(module.ptr().getattr(REGISTRY_ATTRIBUTE_NAME))
{
    if (!_ptr) {
        if (install_if_missing) {
            _ptr = PyPtr::steal(new RegistryImpl());
            module.add(REGISTRY_ATTRIBUTE_NAME, _ptr);
        } else {
            builtin::AttributeError::raise("No bp3 registry found"); // TODO better error message
        }
    } else if (_ptr->ob_type != &RegistryImpl::type) {
        builtin::TypeError::raise("Object is not a bp3.Registry instance"); // TODO better error message
    }
}

std::shared_ptr<Registration> Registry::lookup(bp3::TypeInfo const & t) const {
    Registration::Map & map = getMap(this);
    Registration::Map::const_iterator iter = map.find(t);
    std::shared_ptr<Registration> result;
    if (iter != map.end()) result = iter->second;
    return result;
}

void Registry::import(Registry const & other) const {
    Registration::Map & this_map = getMap(this);
    Registration::Map & other_map = getMap(&other);
    for (auto other_item : other_map) {
        // We don't want to just transfer the shared_ptr; each module should have its
        // own registration for a particular type, so registering new converters for
        // that type doesn't affect the imported modules.
        std::shared_ptr<Registration> & this_reg = this_map[other_item.first];
        if (!this_reg) {
            this_reg = std::make_shared<Registration>();
        }
        std::shared_ptr<Registration> other_reg = other_item.second;
        this_reg->from_python.insert(
            this_reg->from_python.begin(),
            other_reg->from_python.begin(),
            other_reg->from_python.end()
        );
        for (auto other_derived : other_reg->derived) {
            // Instead of transferring the derived-class registrations directly from other_map, we lookup
            // the corresponding registrations in this_map, and ensure those are in this_reg->derived.
            this_reg->derived[other_derived.first] = this_map[other_derived.first];
        }
    }
}

void Registry::registerFromPython(
    TypeInfo const & t, bool is_lvalue,
    FromPythonCheckFunc check,
    FromPythonConvertFunc convert,
    FromPythonPostcallFunc postcall,
    FromPythonCleanupFunc cleanup
) const {
    std::shared_ptr<Registration> & reg = getMap(this)[t];
    if (!reg) {
        reg = std::make_shared<Registration>();
    }
    reg->from_python.push_front(
        FromPythonFuncs{is_lvalue, check, convert, postcall, cleanup}
    );
}

PyPtr Registry::initType() {
    PyPtr r;
    if (PyType_Ready(&RegistryImpl::type) == 0) {
        r = PyPtr::borrow(reinterpret_cast<PyObject*>(&RegistryImpl::type));
    }
    return r;
}

} // namespace bp3
