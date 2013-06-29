#ifndef BP3_CONVERSION_from_python_base_hpp_INCLUDED
#define BP3_CONVERSION_from_python_base_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/TypeInfo.hpp"

namespace bp3 { 

class Module;

class converter_data {
public:
    void * scratch1;
    void * scratch2;
};

namespace conversion  {

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
typedef int (*from_python_check_func)(PyPtr const &, converter_data &);

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
typedef void * (*from_python_convert_func)(PyPtr const &, converter_data &);

/*
   Postcall is only called if the convert function is also called, and
   converters are not required to have a postcall function at all.
*/
typedef void (*from_python_postcall_func)(PyPtr const &, converter_data &);

/*
   Cleanup is called whenever the check function does not throw.  It
   must be able to successfully cleanup whether or not the convert
   function is called, and if it is called, whether or not it
   succeeds.  Cleanup must not throw.
*/
typedef void (*from_python_cleanup_func)(converter_data &);

class from_python_funcs;

class from_python_base {
public:

    explicit from_python_base(
        Module const & mod, PyPtr const & py, bp3::TypeInfo const & ti, bool is_lvalue
    );

    from_python_base(from_python_base const &) = delete;

    from_python_base & operator=(from_python_base const &) = delete;
    
    bool is_convertible() const { return _penalty >= 0; }

    int penalty() const { return _penalty; }

    void postcall();

    ~from_python_base();

protected:
    void * _convert();
private:
    PyPtr _py;
    int _penalty;
    converter_data _data;
    from_python_funcs * _funcs;
};

}} // namespace bp3::conversion

#endif // !BP3_CONVERSION_from_python_base_hpp_INCLUDED
