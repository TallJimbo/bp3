#ifndef BP3_FromPython_hpp_INCLUDED
#define BP3_FromPython_hpp_INCLUDED

#include "bp3/FromPythonBase.hpp"
#include "bp3/TypeInfo.hpp"

namespace bp3 {

template <typename T>
class FromPython : public FromPythonBase {
public:

    explicit FromPython(Registry const & registry, PyPtr const & py) :
        FromPythonBase(registry, py, makeTypeInfo<T>(), false)
    {}

    T convert() { return *reinterpret_cast<T*>(this->_convert()); }

};

template <typename T>
class FromPython<T &> : public FromPythonBase {
public:

    explicit FromPython(Registry const & registry, PyPtr const & py) :
        FromPythonBase(registry, py, makeTypeInfo<T>(), std::is_const<T>::value)
    {}

    T & convert() { return *reinterpret_cast<T*>(this->_convert()); }

};

template <typename T>
class FromPython<T *> : public FromPythonBase {
public:

    explicit FromPython(Registry const & registry, PyPtr const & py) :
        FromPythonBase(registry, py, makeTypeInfo<T>(), std::is_const<T>::value)
    {}

    T * convert() { return reinterpret_cast<T*>(this->_convert()); }

};

template <typename T>
class FromPython<T * const> : public FromPythonBase {
public:

    explicit FromPython(Registry const & registry, PyPtr const & py) :
        FromPythonBase(registry, py, makeTypeInfo<T>(), std::is_const<T>::value)
    {}

    T * convert() { return reinterpret_cast<T*>(this->_convert()); }

};

} // namespace bp3

#endif // !BP3_FromPython_hpp_INCLUDED
