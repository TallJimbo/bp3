#ifndef BP3_from_python_funcs_hpp_INCLUDED
#define BP3_from_python_funcs_hpp_INCLUDED

#include "bp3/PyPtr.hpp"

namespace bp3 {

class ConverterData {
public:
    void * scratch1;
    void * scratch2;
};

/*
   The returned int is a penalty that indicates how close this conversion is to an exact match.  We
   always use the converter with the lowest (nonnegative) penalty, and we add up the penalties for
   function signature when comparing overloads; the overload with the lowest penalty total is chosen.
   penalty < 0:  Not convertible using this converter.
   penalty == 0: Perfect match; this converter should be used, and we can stop searching for others.
   penalty > 0:  Imperfect match; this converter can be used if we can't find a better one.

   When a class has registered derived classes and we have an imperfect match, we will also search
   those derived class registrations for converters until we find a perfect match.  If we don't find a
   perfect match, the most-derived imperfect conversion is used, and the number of inheritance
   levels is added to the final penalty used in overload selection.  This allows base class converters
   to simply return 1 when encountering a derived class instance, and the system will determine
   the correct penalty.  Because derived classes are only searched when the base class reports
   and imperfect match, we avoid traversing unnecessary branches of the inheritance tree.

   If the check function throws an exception, the exception will be caught and ignored, and the call
   will be treated as though a negative value was returned; the cleanup function will not be called.
*/
typedef int (*FromPythonCheckFunc)(PyPtr const &, ConverterData &);

/*
   Function that does the conversion; the returned pointer points to
   an instance of the type the converter was registered with.  The
   pointer may be allocated by the converter (in either the check
   function or the convert function), in which case it must be
   deallocated by the cleanup function, or it may point to memory in
   the Python object.

   If the conversion fails, it should use builtin::Exception::raise
   (or the same member function on an Exception subclass); calling
   code will ensure the cleanup function (if provided) is called.
*/
typedef void * (*FromPythonConvertFunc)(PyPtr const &, ConverterData &);

/*
   Postcall is only called if the convert function is also called, and
   converters are not required to have a postcall function at all.
*/
typedef void (*FromPythonPostcallFunc)(PyPtr const &, ConverterData &);

/*
   Cleanup is called whenever the check function does not throw.  It
   must be able to successfully cleanup whether or not the convert
   function is called, and if it is called, whether or not it
   succeeds.  Cleanup must not throw.
*/
typedef void (*FromPythonCleanupFunc)(ConverterData &);

class FromPythonFuncs {
public:
    bool is_lvalue;
    FromPythonCheckFunc check;
    FromPythonConvertFunc convert;
    FromPythonPostcallFunc postcall;
    FromPythonCleanupFunc cleanup;
};

} // namespace bp3

#endif // !BP3_FromPythonFuncs_hpp_INCLUDED
