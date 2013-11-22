#include "bp3/Callable.hpp"
#include "bp3/Registration.hpp"
#include <sstream>
#include <list>
#include <map>
#include <cstring>

namespace bp3 {

namespace {

struct compareTypeInfo {
    bool operator()(bp3::TypeInfo const & a, bp3::TypeInfo const & b) const {
        return std::strcmp(a.name(), b.name()) < 0;
    }
};

typedef std::map<bp3::TypeInfo,std::shared_ptr<Registration>,compareTypeInfo> RegistryMap;

struct RegistryImpl : public ::PyObject {

    static PyTypeObject type;

    RegistryImpl() {
        PyObject_Init(this, &type);
    }

    static void destroy(::PyObject * self);

    RegistryMap map;
};

RegistryMap & getMap(Registry const * self) {
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

} // anonymous

Registry::Registry() : _ptr(PyPtr::steal(new RegistryImpl())) {}

std::shared_ptr<Registration> Registry::lookup(bp3::TypeInfo const & t) const {
    RegistryMap & map = getMap(this);
    RegistryMap::const_iterator iter = map.find(t);
    std::shared_ptr<Registration> result;
    if (iter != map.end()) result = iter->second;
    return result;
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
