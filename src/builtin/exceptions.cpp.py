import sys
import os
sys.path.append(os.path.join(os.path.split(__file__)[0], "..", ".."))
import buildutils

factory = "    static ExceptionFactory<builtin::{cls}> f{cls}(&f{base});"

raise_def = """void {cls}::raise(std::string const & msg) {{
    PyErr_SetString(typeobject().ptr().get(), msg.c_str());
    bp3::detail::ExceptionAccess::fetch_and_throw<{cls}>();
}}"""

main = """
#include "bp3/builtin/object.hpp"
#include "bp3/builtin/exceptions.hpp"
#include "bp3/ExceptionAccess.hpp"

namespace bp3 {{

namespace detail {{

ExceptionFactoryBase * ExceptionFactoryBase::root() {{
    static ExceptionFactory<builtin::BaseException> fBaseException;
    {factories}
    return &fBaseException;
}};

}} // namespace detail

namespace builtin {{

{raise_defs}

std::nullptr_t BaseException::release() {{
    PyErr_Restore(type(_value).ptr().incref(), _value.ptr().incref(), _traceback.incref());
    return nullptr;
}}

}} // namespace builtin

void throw_error_already_set() {{
    PyPtr type = PyPtr::borrow(PyErr_Occurred());
    if (!type) {{
        builtin::SystemError::raise("C++ exception throw requested with no Python exception set.");
    }}
    detail::ExceptionFactoryBase::search(type)->fetch_and_throw();
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
