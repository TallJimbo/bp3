#include "bp3/callable.hpp"

namespace bp3 {

overload_base::overload_base(std::size_t n_args, arg_def_list const & arg_defs) : _arg_defs(arg_defs) {
    _arg_defs.validate(n_args);
}

void overload_base::unpack_args(
    std::string const & name,
    PyPtr const & args, PyPtr const & kwds,
    overload_data & data, bool throw_on_failure
) const {
    data.unpack_successful = _arg_defs.parse(name, args, kwds, data.unpacked_args, throw_on_failure);
}

PyPtr callable::call(PyPtr const & args, PyPtr const & kwds) const {
    try {
        if (_overloads.size() == static_cast<std::size_t>(1)) {
            overload_data data;
            _overloads.front()->unpack_args(_name, args, kwds, data, true);
            assert(data.unpack_successful);  // or we should have thrown
            _overloads.front()->convert_args(_mod, data);
            // TODO: actually calling the C++ func.
        } else {
            std::vector<overload_data> data(_overloads.size());
            for (std::size_t i = 0; i < _overloads.size(); ++i) {
                _overloads[i]->unpack_args(_name, args, kwds, data[i], false);
                if (data[i].unpack_successful) {
                    _overloads[i]->convert_args(_mod, data[i]);
                }
            }
            // TODO: actually calling the C++ func;
        }
        return PyPtr::borrow(Py_None);
    } catch (builtin::Exception & err) {
        err.release();
        return PyPtr();
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "unknown C++ exception");
        return PyPtr();
    }
}

} // namespace bp3
