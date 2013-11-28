#include "bp3/Module.hpp"
#include "bp3/FromPythonBase.hpp"
#include "bp3/Registration.hpp"
#include "bp3/Debug.hpp"
#include "bp3/builtin/str.hpp"

namespace bp3 {

PyPtr ToPython::_move(TypeInfo const & ti, Registry const & registry, void * value) {
    std::shared_ptr<Registration> reg = registry.lookup(ti);
    if (reg->move_to_python) {
        return reg->move_to_python(value);
    } else {
        // throw exception
    }
}

PyPtr ToPython::_ref(TypeInfo const & ti, Registry const & registry, void * value) {
    std::shared_ptr<Registration> reg = registry.lookup(ti);
    if (reg->ref_to_python) {
        return reg->ref_to_python(value);
    } else {
        // throw exception
    }
}

PyPtr ToPython::_cref(TypeInfo const & ti, Registry const & registry, void const * value) {
    std::shared_ptr<Registration> reg = registry.lookup(ti);
    if (reg->cref_to_python) {
        return reg->cref_to_python(value);
    } else {
        // throw exception
    }
}

} // namespace bp3
