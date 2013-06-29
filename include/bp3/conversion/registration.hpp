#ifndef BP3_CONVERSION_registration_hpp_INCLUDED
#define BP3_CONVERSION_registration_hpp_INCLUDED

#include "bp3/PyPtr.hpp"
#include "bp3/TypeInfo.hpp"
#include "bp3/conversion/from_python_funcs.hpp"

#include <list>

namespace bp3 { namespace conversion  {

class registration {
public:

    registration();

    std::list<std::shared_ptr<std::list<from_python_funcs>>> from_python;

    std::list<std::shared_ptr<registration>> derived;

};

}} // namespace bp3::conversion

#endif // !BP3_CONVERSION_registration_hpp_INCLUDED
