#include "bp3/callables/Callable.hpp"

namespace bp3 {
namespace callables {

PyPtr Callable::call(PyPtr const & args, PyPtr const & kwds) const {
    try {
        if (_overloads.size() == static_cast<std::size_t>(1)) {
            OverloadData data;
            _overloads.front()->unpackArgs(_name, args, kwds, data, true);
            assert(data.unpack_successful);  // or we should have thrown
            _overloads.front()->convertArgs(_mod, data);
            // TODO: actually calling the C++ func.
        } else {
            std::vector<OverloadData> data(_overloads.size());
            for (std::size_t i = 0; i < _overloads.size(); ++i) {
                _overloads[i]->unpackArgs(_name, args, kwds, data[i], false);
                if (data[i].unpack_successful) {
                    _overloads[i]->convertArgs(_mod, data[i]);
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

} // namespace callables
} // namespace bp3
