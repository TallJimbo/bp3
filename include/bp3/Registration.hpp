#ifndef BP3_Registration_hpp_INCLUDED
#define BP3_Registration_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/TypeInfo.hpp"
#include "bp3/FromPythonFuncs.hpp"

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

    Registration() {}

    typedef std::list<FromPythonFuncs> FromPythonList;
    typedef std::map<TypeInfo,std::shared_ptr<Registration>,CompareTypeInfo> Map;

    FromPythonList from_python;

    Map derived;

};

} // namespace bp3

#endif // !BP3_Registration_hpp_INCLUDED
