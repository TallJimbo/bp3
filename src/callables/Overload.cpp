#include "bp3/callables/Overload.hpp"

namespace bp3 {
namespace callables {

OverloadBase::OverloadBase(std::size_t n_args, ArgDefList const & arg_defs) : _arg_defs(arg_defs) {
    _arg_defs.validate(n_args);
}

void OverloadBase::unpackArgs(
    std::string const & name,
    PyPtr const & args, PyPtr const & kwds,
    OverloadData & data, bool throw_on_failure
) const {
    data.unpack_successful = _arg_defs.parse(name, args, kwds, data.unpacked_args, throw_on_failure);
}

} // namespace callables
} // namespace bp3
