#include "bp3/conversion/registration.hpp"

namespace bp3 { namespace conversion {

registration::registration() {
    from_python.push_front(std::make_shared<std::list<from_python_funcs>>());
}

}} // namespace bp3::conversion
