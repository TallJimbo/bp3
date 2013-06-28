import sys
import os
sys.path.append(os.path.join(os.path.split(__file__)[0], "..", "..", ".."))
import buildutils

class_decl = """
class {cls} : public {base} {{
public:
    {cls}({cls} const & other) : {base}(other) {{}}
    static type typeobject() {{ return type(py_ptr::borrow(PyExc_{cls})); }}
    static void raise(std::string const & what);
protected:
    friend class detail::exception_access;
    {cls}(py_ptr const & value, py_ptr const & traceback) : {base}(value, traceback) {{}}
}};"""

main = """
#ifndef BP3_BUILTIN_exceptions_hpp_INCLUDED
#define BP3_BUILTIN_exceptions_hpp_INCLUDED

// THIS FILE IS MACHINE-GENERATED.  DO NOT EDIT DIRECTLY.

#include "bp3/builtin/object.hpp"

namespace bp3 {{

void throw_error_already_set();

namespace builtin {{

namespace detail {{

class exception_access;

}} // namespace detail

class BaseException {{
public:

    BaseException & operator=(BaseException const &) = delete;

    py_ptr const & ptr() const {{ return _value.ptr(); }}

    operator object const & () const {{ return _value; }}

    std::nullptr_t release();

    static void raise(std::string const & msg);

    static object typeobject() {{ return type(py_ptr::borrow(PyExc_BaseException)); }}

protected:

    friend class detail::exception_access;

    BaseException(py_ptr const & value, py_ptr const & traceback) : _value(value), _traceback(traceback) {{}}

    object _value;
    py_ptr _traceback;
}};

{class_decls}

}}}} // namespace bp3::builtin

#endif // !BP3_BUILTIN_exceptions_hpp_INCLUDED
"""

path = os.path.split(sys.argv[1])[0]
if not os.path.exists(path): os.makedirs(path)
with open(sys.argv[1], "w") as stream:
    classes = buildutils.get_builtin_exceptions()
    class_decls = "\n".join(class_decl.format(cls=cls.__name__, base=cls.__base__.__name__)
                            for cls in classes[1:])
    stream.write(main.format(class_decls = class_decls))
