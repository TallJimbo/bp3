#ifndef BP3_Registration_hpp_INCLUDED
#define BP3_Registration_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/TypeInfo.hpp"
#include "bp3/FromPythonFuncs.hpp"

#include <list>

namespace bp3 {

class Registration {
public:

    Registration() {}

    std::list<FromPythonFuncs> from_python;

    std::list<std::shared_ptr<Registration>> derived;

};

} // namespace bp3

#endif // !BP3_Registration_hpp_INCLUDED
