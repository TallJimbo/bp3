#ifndef BP3_BUILTIN_ExceptionAccess_hpp_INCLUDED
#define BP3_BUILTIN_ExceptionAccess_hpp_INCLUDED

#include "bp3/builtin/object.hpp"

namespace bp3 { namespace detail {

class ExceptionAccess {
public:

    template <typename ExceptionT>
    static void fetch_and_throw() {
        PyObject* ptype = nullptr;
        PyObject* pvalue = nullptr;
        PyObject* ptraceback = nullptr;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
        PyPtr type = PyPtr::steal(ptype);
        PyPtr value = PyPtr::steal(pvalue);
        PyPtr traceback = PyPtr::steal(ptraceback);
        throw ExceptionT(value, traceback);
    }

};

enum MatchEnum { NONE, EXACT, DERIVED };

struct ExceptionFactoryBase {

    static ExceptionFactoryBase * root();

    static ExceptionFactoryBase * search(PyPtr const & type);

    virtual MatchEnum match(PyPtr const & type) const = 0;

    virtual void fetch_and_throw() const = 0;

    virtual ~ExceptionFactoryBase() {}

    ExceptionFactoryBase() : sibling(nullptr), derived(nullptr) {}

    ExceptionFactoryBase * sibling;
    ExceptionFactoryBase * derived;

};

template <typename ExceptionT>
struct ExceptionFactory : public ExceptionFactoryBase {

    virtual void fetch_and_throw() const {
        ExceptionAccess::fetch_and_throw<ExceptionT>();
    }

    virtual MatchEnum match(PyPtr const & type) const {
        if (type.get() == ExceptionT::typeobject().ptr().get()) return EXACT;
        if (PyObject_IsSubclass(type.get(), ExceptionT::typeobject().ptr().get())) return DERIVED;
        return NONE;
    }

    explicit ExceptionFactory(ExceptionFactoryBase * base = nullptr) {
        if (!base) return;
        if (base->derived) {
            ExceptionFactoryBase * c = base->derived;
            while (c->sibling) c = c->sibling;
            c->sibling = this;
        } else {
            base->derived = this;
        }
    }

};

}} // namespace bp3::detail

#endif // !BP3_BUILTIN_exceptions_hpp_INCLUDED
