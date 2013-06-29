#include "bp3/Registration.hpp"

namespace bp3 {

Registration::Registration() {
    from_python.push_front(std::make_shared<std::list<FromPythonFuncs>>());
}

} // namespace bp3
