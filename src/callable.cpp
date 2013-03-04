#include "bp3/callable.hpp"

namespace bp3 {

namespace {

struct overload_data {
    bool unpack_successful;
    std::vector<py_ptr> unpacked_args;
};

} // anonymous

overload_base::overload_base(std::size_t n_args, arg_list const & kwargs) : _kwargs(kwargs) {
    _kwargs.validate(n_args);
}

py_ptr callable::call(py_ptr const & args, py_ptr const & kwds) const {
    try {
        if (_overloads.size() == static_cast<std::size_t>(1)) {
            std::vector<py_ptr> unpacked_args;
            _overloads.front()->_kwargs.parse(_name, args, kwds, unpacked_args, true);
            // TODO: conversions, actually calling the C++ func.
        } else {
            std::vector<overload_data> data(_overloads.size());
            for (std::size_t i = 0; i < _overloads.size(); ++i) {
                data[i].unpack_successful =
                    _overloads[i]->_kwargs.parse(_name, args, kwds, data[i].unpacked_args, false);
            }
            // TODO: conversions, actually calling the C++ func;
        }
        return py_ptr::borrow(Py_None);
    } catch (builtin::Exception & err) {
        err.release();
        return py_ptr();
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "unknown C++ exception");
        return py_ptr();
    }
}

} // namespace bp3
