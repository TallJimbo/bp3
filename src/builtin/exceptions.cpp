#include "bp3/builtin/object.hpp"
#include "bp3/builtin/exceptions.hpp"

namespace bp3 { namespace builtin {

namespace exceptions {

class access {
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

struct factory_base {

    static factory_base * root();

    static factory_base * search(py_ptr const & type);

    virtual MatchEnum match(py_ptr const & type) const = 0;

    virtual void fetch_and_throw() const = 0;

    virtual ~factory_base() {}

    explicit factory_base() : sibling(nullptr), derived(nullptr) {}

    factory_base * sibling;
    factory_base * derived;
};

factory_base * factory_base::search(py_ptr const & target) {
    factory_base * current = root();
    factory_base * best = current;
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
struct factory : public factory_base {
    
    virtual void fetch_and_throw() const {
        access::fetch_and_throw<ExceptionT>();
    }

    virtual MatchEnum match(py_ptr const & type) const {
        if (type.get() == ExceptionT::type().ptr().get()) return EXACT;
        if (PyObject_IsSubclass(type.get(), ExceptionT::type().ptr().get())) return DERIVED;
        return NONE;
    }

    explicit factory(factory_base * base = nullptr) {
        if (!base) return;
        if (base->derived) {
            factory_base * c = base->derived;
            while (c->sibling) c = c->sibling;
            c->sibling = this;
        } else {
            base->derived = this;
        }
    }

};

factory_base * factory_base::root() {
    static factory<Exception> fException;
    static factory<StandardError> fStandardError(&fException);
    static factory<SystemError> fSystemError(&fStandardError);
    static factory<TypeError> fTypeError(&fStandardError);
    return &fException;
};

} // anonymous

} // namespace exceptions

void throw_error_already_set() {
    py_ptr type = py_ptr::borrow(PyErr_Occurred());
    if (!type) {
        SystemError::raise("C++ exception throw requested with no Python exception set.");
    }
    exceptions::factory_base::search(type)->fetch_and_throw();
}

void Exception::raise_impl(std::string const & what, object const & type) {
    PyErr_SetString(type.ptr().get(), what.c_str());
    throw_error_already_set();
}

}} // namespace bp3::builtin
