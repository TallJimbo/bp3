#ifndef BP3_FromPythonBase_hpp_INCLUDED
#define BP3_FromPythonBase_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/TypeInfo.hpp"
#include "bp3/FromPythonFuncs.hpp"

namespace bp3 {

class Registry;

class FromPythonBase {
public:

    explicit FromPythonBase(
        Registry const & registry, PyPtr const & py, bp3::TypeInfo const & ti, bool is_lvalue
    );

    FromPythonBase(FromPythonBase const &) = delete;

    FromPythonBase(FromPythonBase &&) = delete;

    FromPythonBase & operator=(FromPythonBase const &) = delete;

    FromPythonBase & operator=(FromPythonBase &&) = delete;

    bool isConvertible() const { return _penalty >= 0; }

    int getPenalty() const { return _penalty; }

    PyPtr getPyObject() const { return _py; }

    void postcall();

    ~FromPythonBase();

protected:
    void * _convert();
private:
    PyPtr _py;
    int _penalty;
    ConverterData _data;
    FromPythonFuncs * _funcs;
};

} // namespace bp3

#endif // !BP3_FromPythonBase_hpp_INCLUDED
