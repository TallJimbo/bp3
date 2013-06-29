#include "bp3/builtin/numbers.hpp"
#include "bp3/builtin/exceptions.hpp"

namespace bp3 {  namespace builtin {

float_::float_(object const & obj) :
    base(PyPtr::steal(PyNumber_Float(obj.ptr().get()))) {}

float_::float_(double v) :
    base(PyPtr::steal(PyFloat_FromDouble(v))) {}

float_::operator double () const { return PyFloat_AsDouble(ptr().get()); }


#if PY_MAJOR_VERSION == 2

int_::int_(object const & obj) :
    base(PyPtr::steal(PyNumber_Int(obj.ptr().get()))) {}

int_::int_(long v) :
    base(PyPtr::steal(PyInt_FromLong(v))) {}

int_::operator long () const { return PyInt_AsLong(ptr().get()); }


long_::long_(object const & obj) :
    base(PyPtr::steal(PyNumber_Long(obj.ptr().get()))) {}

long_::long_(long long v) :
    base(PyPtr::steal(PyLong_FromLongLong(v))) {}

long_::long_(unsigned long long v) :
    base(PyPtr::steal(PyLong_FromUnsignedLongLong(v))) {}

long_::operator long long () const { return PyLong_AsLongLong(ptr().get()); }

long_::operator unsigned long long () const { return PyLong_AsUnsignedLongLong(ptr().get()); }

#else

int_::int_(object const & obj) :
    base(PyPtr::steal(PyNumber_Long(obj.ptr().get()))) {}

int_::int_(long long v) :
    base(PyPtr::steal(PyLong_FromLongLong(v))) {}

int_::int_(unsigned long long v) :
    base(PyPtr::steal(PyLong_FromUnsignedLongLong(v))) {}

int_::operator long long () const { return PyLong_AsLongLong(ptr().get()); }

int_::operator unsigned long long () const { return PyLong_AsUnsignedLongLong(ptr().get()); }

#endif

}} // namespace bp3::builtin
