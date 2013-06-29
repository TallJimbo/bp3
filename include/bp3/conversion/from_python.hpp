#ifndef BP3_CONVERSION_from_python_hpp_INCLUDED
#define BP3_CONVERSION_from_python_hpp_INCLUDED

#include "bp3/conversion/from_python_base.hpp"

#include <typeinfo>

namespace bp3 { namespace conversion  {

template <typename T>
class from_python : public from_python_base {
public:

    explicit from_python(module const & mod, PyPtr const & py) :
        from_python_base(mod, py, makeTypeInfo<T>(), false)
    {}

    T convert() { return *reinterpret_cast<T*>(this->_convert()); }

};

template <typename T>
class from_python<T &> : public from_python_base {
public:

    explicit from_python(module const & mod, PyPtr const & py) :
        from_python_base(mod, py, makeTypeInfo<T>(), std::is_const<T>::value)
    {}

    T & convert() { return *reinterpret_cast<T*>(this->_convert()); }

};

template <typename T>
class from_python<T *> : public from_python_base {
public:

    explicit from_python(module const & mod, PyPtr const & py) :
        from_python_base(mod, py, makeTypeInfo<T>(), std::is_const<T>::value)
    {}

    T * convert() { return reinterpret_cast<T*>(this->_convert()); }
    
};

template <typename T>
class from_python<T * const> : public from_python_base {
public:

    explicit from_python(module const & mod, PyPtr const & py) :
        from_python_base(mod, py, makeTypeInfo<T>(), std::is_const<T>::value)
    {}

    T * convert() { return reinterpret_cast<T*>(this->_convert()); }
    
};

}} // namespace bp3::conversion

#endif // !BP3_CONVERSION_from_python_hpp_INCLUDED
