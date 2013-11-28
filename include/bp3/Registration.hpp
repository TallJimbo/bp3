#ifndef BP3_Registration_hpp_INCLUDED
#define BP3_Registration_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/TypeInfo.hpp"
#include "bp3/FromPythonFuncs.hpp"
#include "bp3/ToPython.hpp"

#include <list>
#include <map>
#include <cstring>

namespace bp3 {

class Registration {

    struct CompareTypeInfo {
        bool operator()(TypeInfo const & a, TypeInfo const & b) const {
            return std::strcmp(a.name(), b.name()) < 0;
        }
    };

public:

    Registration() : move_to_python(nullptr), ref_to_python(nullptr), cref_to_python(nullptr) {}

    typedef std::list<FromPythonFuncs> FromPythonList;
    typedef std::map<TypeInfo,std::shared_ptr<Registration>,CompareTypeInfo> Map;

    MoveToPythonFunc move_to_python;

    RefToPythonFunc ref_to_python;

    CRefToPythonFunc cref_to_python;

    FromPythonList from_python;

    Map derived;

};

} // namespace bp3

#endif // !BP3_Registration_hpp_INCLUDED
