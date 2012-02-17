#include "bp3/builtin/object.hpp"
#include "bp3/builtin/exceptions.hpp"

namespace bp3 { namespace builtin {

namespace detail {

class exception_access {
public:
    
    template <typename ExceptionT>
    static void fetch_and_throw() {
        PyObject* ptype = nullptr;
        PyObject* pvalue = nullptr;
        PyObject* ptraceback = nullptr;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
        py_ptr type = py_ptr::steal(ptype);
        py_ptr value = py_ptr::steal(pvalue);
        py_ptr traceback = py_ptr::steal(ptraceback);
        throw ExceptionT(value, traceback);
    }

};

namespace {

enum MatchEnum { NONE, EXACT, DERIVED };

struct exception_factory_base {

    static exception_factory_base * root();

    static exception_factory_base * search(py_ptr const & type);

    virtual MatchEnum match(py_ptr const & type) const = 0;

    virtual void fetch_and_throw() const = 0;

    virtual ~exception_factory_base() {}

    explicit exception_factory_base() : sibling(nullptr), derived(nullptr) {}

    exception_factory_base * sibling;
    exception_factory_base * derived;
};

exception_factory_base * exception_factory_base::search(py_ptr const & target) {
    exception_factory_base * current = root();
    exception_factory_base * best = current;
    while (true) {
        MatchEnum match = current->match(target);
        if (match == EXACT) return current;
        if (match == DERIVED) {
            best = current;
            if (current->derived) {
                current = current->derived;
            } else {
                return best;
            }
        } else if (current->sibling) {
            current = current->sibling;
        } else {
            return best;
        }
    }
}

template <typename ExceptionT>
struct exception_factory : public exception_factory_base {
    
    virtual void fetch_and_throw() const {
        exception_access::fetch_and_throw<ExceptionT>();
    }

    virtual MatchEnum match(py_ptr const & type) const {
        if (type.get() == ExceptionT::typeobject().ptr().get()) return EXACT;
        if (PyObject_IsSubclass(type.get(), ExceptionT::typeobject().ptr().get())) return DERIVED;
        return NONE;
    }

    explicit exception_factory(exception_factory_base * base = nullptr) {
        if (!base) return;
        if (base->derived) {
            exception_factory_base * c = base->derived;
            while (c->sibling) c = c->sibling;
            c->sibling = this;
        } else {
            base->derived = this;
        }
    }

};

exception_factory_base * exception_factory_base::root() {
    static exception_factory<Exception> fException;
#if PY_MAJOR_VERSION == 2
    static exception_factory<StandardError> fStandardError(&fException);
#else
    exception_factory_base & fStandardError = fException;
#endif
    static exception_factory<SystemError> fSystemError(&fStandardError);
    static exception_factory<TypeError> fTypeError(&fStandardError);
    return &fException;
};

} // anonymous

} // namespace detail

#define BP3_BUILTIN_EXCEPTION_RAISE(cls)        \
    void cls::raise(std::string const & msg) {  \
        PyErr_SetString(typeobject().ptr().get(), msg.c_str()); \
        detail::exception_access::fetch_and_throw<cls>();   \
    }

BP3_BUILTIN_EXCEPTION_RAISE(Exception)
#if PY_MAJOR_VERSION == 2
BP3_BUILTIN_EXCEPTION_RAISE(StandardError)
#endif
BP3_BUILTIN_EXCEPTION_RAISE(SystemError)
BP3_BUILTIN_EXCEPTION_RAISE(TypeError)

} // namespace builtin

void throw_error_already_set() {
    py_ptr type = py_ptr::borrow(PyErr_Occurred());
    if (!type) {
        builtin::SystemError::raise("C++ exception throw requested with no Python exception set.");
    }
    builtin::detail::exception_factory_base::search(type)->fetch_and_throw();
}

} // namespace bp3
