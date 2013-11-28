#ifndef BP3_Registry_hpp_INCLUDED
#define BP3_Registry_hpp_INCLUDED

#include "bp3/TypeInfo.hpp"
#include "bp3/FromPythonBase.hpp"
#include "bp3/ToPython.hpp"
#include "bp3/builtin/exceptions.hpp"

#include <string>
#include <memory>

namespace bp3 {

class Registration;
class Module;

class Registry {
public:

    explicit Registry(Module const & module, bool install_if_missing=false);

    std::shared_ptr<Registration> lookup(TypeInfo const & t) const;

    void import(Registry const & other) const;

    void registerMoveToPython(
        TypeInfo const & t,
        MoveToPythonFunc func
    ) const;

    void registerRefToPython(
        TypeInfo const & t,
        RefToPythonFunc func
    ) const;

    void registerCRefToPython(
        TypeInfo const & t,
        CRefToPythonFunc func
    ) const;

    void registerFromPython(
        TypeInfo const & t, bool is_lvalue,
        FromPythonCheckFunc check,
        FromPythonConvertFunc convert,
        FromPythonPostcallFunc postcall,
        FromPythonCleanupFunc cleanup=nullptr
    ) const;

    void registerFromPython(
        TypeInfo const & t, bool is_lvalue,
        FromPythonCheckFunc check,
        FromPythonConvertFunc convert,
        FromPythonCleanupFunc cleanup=nullptr
    ) const {
        registerFromPython(t, is_lvalue, check, convert, nullptr, cleanup);
    }

    PyPtr const & ptr() const { return _ptr; }

private:

    friend class LibraryAccess;

    static PyPtr initType();

    PyPtr _ptr;
};

} // namespace bp3

#endif // !BP3_Registry_hpp_INCLUDED
