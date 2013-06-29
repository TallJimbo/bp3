#include "bp3/builtin/str.hpp"
#include "bp3/builtin/exceptions.hpp"

namespace bp3 {  namespace builtin {

bytes::bytes() : base(PyPtr::steal(PyBytes_FromString(""))) {}

bytes::bytes(object const & obj) :
    base(PyPtr::steal(PyObject_Bytes(obj.ptr().get()))) {}

bytes::bytes(char const * s) :
    base(PyPtr::steal(PyBytes_FromString(s))) {}

bytes::bytes(std::string const & s) :
    base(PyPtr::steal(PyBytes_FromStringAndSize(s.data(), s.size()))) {}

bytes::operator std::string () const { return PyBytes_AS_STRING(ptr().get()); }

char const * bytes::c_str() const { return PyBytes_AS_STRING(ptr().get()); }

#if PY_MAJOR_VERSION == 2
bytes bytes::operator%(tuple const & args) const {
    return bytes(PyPtr::steal(PyString_Format(ptr().get(), args.ptr().get())));
}
#endif



unicode::unicode() : base(PyPtr::steal(PyUnicode_FromString(""))) {}

unicode::unicode(object const & obj) :
    base(PyPtr::steal(
#if PY_MAJOR_VERSION == 2               
               PyObject_Unicode
#else
               PyObject_Str
#endif
               (obj.ptr().get()))) {}

unicode::unicode(char const * s) :
    base(PyPtr::steal(PyUnicode_FromString(s))) {}

unicode::unicode(std::string const & s) : 
    base(PyPtr::steal(PyUnicode_FromStringAndSize(s.data(), s.size()))) {}

unicode::operator std::string () const {
    return bytes(PyPtr::steal(PyUnicode_AsUTF8String(ptr().get())));
}

unicode unicode::operator%(tuple const & args) const {
    return unicode(PyPtr::steal(PyUnicode_Format(ptr().get(), args.ptr().get())));
}



str repr(object const & obj) {
    return str(PyPtr::steal(PyObject_Repr(obj.ptr().get())));
}

}} // namespace bp3::builtin
