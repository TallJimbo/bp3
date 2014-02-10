#ifndef BP3_ToPython_hpp_INCLUDED
#define BP3_ToPython_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/TypeInfo.hpp"

namespace bp3 {

typedef PyPtr (*MoveToPythonFunc)(void *);
typedef PyPtr (*RefToPythonFunc)(void *);
typedef PyPtr (*CRefToPythonFunc)(void const *);

struct ToPython {

    template <typename T>
    static PyPtr copy(Registry const & registry, T value) {
        return _move(makeTypeInfo<T>(), registry, &value);
    }

    template <typename T>
    static PyPtr move(Registry const & registry, T && value) {
        return _move(makeTypeInfo<T>(), registry, &value);
    }

    template <typename T>
    static PyPtr ref(Registry const & registry, T & value) {
        return ref(registry, &value);
    }

    template <typename T>
    static PyPtr ref(Registry const & registry, T const & value) {
        return cref(registry, &value);
    }

    template <typename T>
    static PyPtr ref(Registry const & registry, T * value) {
        return _ref(makeTypeInfo<T>(), registry, value);
    }

    template <typename T>
    static PyPtr ref(Registry const & registry, T const * value) {
        return _cref(makeTypeInfo<T>(), registry, value);
    }

    template <typename T>
    static PyPtr cref(Registry const & registry, T const & value) {
        return cref(registry, &value);
    }

    template <typename T>
    static PyPtr cref(Registry const & registry, T const * value) {
        return _cref(makeTypeInfo<T>(), registry, value);
    }

private:

    static PyPtr _move(TypeInfo const & ti, Registry const & registry, void * value);

    static PyPtr _ref(TypeInfo const & ti, Registry const & registry, void * value);

    static PyPtr _cref(TypeInfo const & ti, Registry const & registry, void const * value);

};

template <typename T>
struct ReturnToPython {

    static PyPtr apply(Registry const & registry, T && value) {
        return ToPython::move(registry, std::move(value));
    }

};

template <typename T>
struct ReturnToPython<T &> {

    static PyPtr apply(Registry const & registry, T & value) {
        return ToPython::ref(registry, value);
    }

};

template <typename T>
struct ReturnToPython<T const &> {

    static PyPtr apply(Registry const & registry, T const & value) {
        return ToPython::cref(registry, value);
    }

};

template <typename T>
struct ReturnToPython<T *> {

    static PyPtr apply(Registry const & registry, T * value) {
        return ToPython::ref(registry, value);
    }

};

template <typename T>
struct ReturnToPython<T const *> {

    static PyPtr apply(Registry const & registry, T const * value) {
        return ToPython::cref(registry, value);
    }

};

} // namespace bp3

#endif // !BP3_ToPython_hpp_INCLUDED
