import sys
import os
sys.path.append(os.path.join(os.path.split(__file__)[0], "..", ".."))
import buildutils

factory = "    static exception_factory<{cls}> f{cls}(&f{base});"

raise_def = """void {cls}::raise(std::string const & msg) {{
    PyErr_SetString(typeobject().ptr().get(), msg.c_str());
    detail::exception_access::fetch_and_throw<{cls}>();
}}"""

main = """
#include "bp3/builtin/object.hpp"
#include "bp3/builtin/exceptions.hpp"

namespace bp3 {{ namespace builtin {{

namespace detail {{

class exception_access {{
public:

    template <typename ExceptionT>
    static void fetch_and_throw() {{
        PyObject* ptype = nullptr;
        PyObject* pvalue = nullptr;
        PyObject* ptraceback = nullptr;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
        py_ptr type = py_ptr::steal(ptype);
        py_ptr value = py_ptr::steal(pvalue);
        py_ptr traceback = py_ptr::steal(ptraceback);
        throw ExceptionT(value, traceback);
    }}

}};

namespace {{

enum MatchEnum {{ NONE, EXACT, DERIVED }};

struct exception_factory_base {{

    static exception_factory_base * root();

    static exception_factory_base * search(py_ptr const & type);

    virtual MatchEnum match(py_ptr const & type) const = 0;

    virtual void fetch_and_throw() const = 0;

    virtual ~exception_factory_base() {{}}

    explicit exception_factory_base() : sibling(nullptr), derived(nullptr) {{}}

    exception_factory_base * sibling;
    exception_factory_base * derived;
}};

exception_factory_base * exception_factory_base::search(py_ptr const & target) {{
    exception_factory_base * current = root();
    exception_factory_base * best = current;
    while (true) {{
        MatchEnum match = current->match(target);
        if (match == EXACT) return current;
        if (match == DERIVED) {{
            best = current;
            if (current->derived) {{
                current = current->derived;
            }} else {{
                return best;
            }}
        }} else if (current->sibling) {{
            current = current->sibling;
        }} else {{
            return best;
        }}
    }}
}}

template <typename ExceptionT>
struct exception_factory : public exception_factory_base {{

    virtual void fetch_and_throw() const {{
        exception_access::fetch_and_throw<ExceptionT>();
    }}

    virtual MatchEnum match(py_ptr const & type) const {{
        if (type.get() == ExceptionT::typeobject().ptr().get()) return EXACT;
        if (PyObject_IsSubclass(type.get(), ExceptionT::typeobject().ptr().get())) return DERIVED;
        return NONE;
    }}

    explicit exception_factory(exception_factory_base * base = nullptr) {{
        if (!base) return;
        if (base->derived) {{
            exception_factory_base * c = base->derived;
            while (c->sibling) c = c->sibling;
            c->sibling = this;
        }} else {{
            base->derived = this;
        }}
    }}

}};

exception_factory_base * exception_factory_base::root() {{
    static exception_factory<BaseException> fBaseException;
    {factories}
    return &fBaseException;
}};

}} // anonymous

}} // namespace detail

{raise_defs}

std::nullptr_t BaseException::release() {{
    PyErr_Restore(type(_value).ptr().incref(), _value.ptr().incref(), _traceback.incref());
    return nullptr;
}}

}} // namespace builtin

void throw_error_already_set() {{
    py_ptr type = py_ptr::borrow(PyErr_Occurred());
    if (!type) {{
        builtin::SystemError::raise("C++ exception throw requested with no Python exception set.");
    }}
    builtin::detail::exception_factory_base::search(type)->fetch_and_throw();
}}

}} // namespace bp3
"""

path = os.path.split(sys.argv[1])[0]
if not os.path.exists(path): os.makedirs(path)
with open(sys.argv[1], "w") as stream:
    classes = buildutils.get_builtin_exceptions()
    factories = "\n".join(factory.format(cls=cls.__name__, base=cls.__base__.__name__)
                          for cls in classes[1:])
    raise_defs = "\n".join(raise_def.format(cls=cls.__name__, base=cls.__base__.__name__)
                           for cls in classes)
    stream.write(main.format(factories=factories, raise_defs=raise_defs))
