#include "bp3/Callable.hpp"
#include <sstream>
#include <list>

namespace bp3 {

namespace {

struct CallableImpl : public ::PyObject {

    static ::PyObject * call(::PyObject * self, ::PyObject * args, ::PyObject * kwds);

    static PyTypeObject type;

    static void destroy(::PyObject * self);

    void addOverload(OverloadPtr overload) { _overloads.push_back(std::move(overload)); }

    CallableImpl(Registry registry, std::string name) :
        _name(std::move(name)), _registry(std::move(registry))
    {
        PyObject_Init(this, &type);
    }

private:
    std::string _name;
    Registry _registry;
    std::vector<OverloadPtr> _overloads;
};

::PyObject * CallableImpl::call(::PyObject * self, ::PyObject * args, ::PyObject * kwds) {
    try {
        if (self->ob_type != &CallableImpl::type) {
            builtin::TypeError::raise("First argument must be a bp3.Callable instance");
        }
        CallableImpl * impl = static_cast<CallableImpl*>(self);
        if (impl->_overloads.size() == static_cast<std::size_t>(1)) {
            OverloadResolutionData data(impl->_overloads.front());
            data.overload->unpackArgs(impl->_name, PyPtr::borrow(args), PyPtr::borrow(kwds), data, true);
            assert(data.unpack_successful);  // or we should have thrown
            data.overload->convertArgs(impl->_registry, data);
            if (!data.converted_args->score.isValid()) {
                std::ostringstream msg;
                msg << "Error in arguments for '" << impl->_name << "'";
                data.converted_args->reportConversionFailure(msg, "\n  ");
                builtin::TypeError::raise(msg.str());
            }
            data.overload->call(data);
        } else {
            typedef std::list<OverloadResolutionData> DataList;
            DataList data(impl->_overloads.begin(), impl->_overloads.end());
            DataList::iterator i = data.begin();
            while (i != data.end()) {
                i->overload->unpackArgs(impl->_name, PyPtr::borrow(args), PyPtr::borrow(kwds), *i, false);
                if (i->unpack_successful) {
                    i->overload->convertArgs(impl->_registry, *i);
                    if (i->converted_args->score.isValid()) {
                        ++i;
                    } else {
                        i = data.erase(i);
                    }
                } else {
                    i = data.erase(i);
                }
                // TODO: save non-matches in so we can use them for diagnostics if nothing succeeds
            }
            if (data.empty()) {
                builtin::TypeError::raise("No matching signatures for call to overloaded function '"
                                          + impl->_name + "'");
            }
            DataList::iterator best = data.begin();
            i = data.begin(); ++i;
            while (i != data.end()) {
                int cmp = best->converted_args->score.compare(i->converted_args->score);
                if (cmp > 0) {
                    data.erase(best);
                    best = i;
                    ++i;
                } else if (cmp < 0) {
                    i = data.erase(i);
                }
            }
            if (data.size() != 1) {
                // TODO: diagnostics for ambiguous calls
                builtin::TypeError::raise("Ambiguous call to overloaded function '" + impl->_name + "'");
            }
            data.front().overload->call(data.front());
        }
        Py_RETURN_NONE;
    } catch (builtin::Exception & err) {
        err.release();
        return nullptr;
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "unknown C++ exception");
        return nullptr;
    }
}

void CallableImpl::destroy(::PyObject * self) {
    delete static_cast<CallableImpl*>(self);
}

PyTypeObject CallableImpl::type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    const_cast<char*>("bp3.Callable"), /*tp_name*/
    sizeof(CallableImpl),      /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)&CallableImpl::destroy,    /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    (ternaryfunc)&CallableImpl::call,       /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "bp3-wrapped C++ function or function object",           /* tp_doc */
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

PyPtr Callable::call(PyPtr const & args, PyPtr const & kwds) const {
    return PyPtr::borrow(CallableImpl::call(_ptr.get(), args.get(), kwds.get())).raise_if_null();
}

void Callable::_initialize(Registry registry, std::string name) {
    _ptr = PyPtr::steal(new CallableImpl(std::move(registry), std::move(name)));
}

void Callable::_addOverload(OverloadPtr overload) {
    CallableImpl * impl = static_cast<CallableImpl*>(_ptr.get());
    impl->addOverload(std::move(overload));
}

PyPtr Callable::initType() {
    PyPtr r;
    if (PyType_Ready(&CallableImpl::type) == 0) {
        r = PyPtr::borrow(reinterpret_cast<PyObject*>(&CallableImpl::type));
    }
    return r;
}

} // namespace bp3
