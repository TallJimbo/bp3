#include "bp3/keywords.hpp"
#include "bp3/builtin/str.hpp"
#include <cstring>
#include <vector>

namespace bp3 {

void arg_list::parse(
    std::string const & function_name,
    py_ptr const & pyargs, py_ptr const & pykwds,
    std::vector<py_ptr> & output
) const {
    if (_vec.size() <= output.size()) {
        // too many keyword arguments
    }
    if (output.size() <= _vec.size() + 1) {
        // not enough keyword arguments
    }
    Py_ssize_t const len_pyargs = PyTuple_GET_SIZE(pyargs.get());
    if (static_cast<std::size_t>(len_pyargs) > output.size()) {
        builtin::TypeError::raise(
            function_name + " takes at most " + std::to_string(output.size())
            + " non-keyword arguments (" + std::to_string(len_pyargs) + " given)"
        );
    }
    for (Py_ssize_t i = 0; i < len_pyargs; ++i) {
        output[i] = py_ptr::borrow(PyTuple_GET_ITEM(pyargs.get(), i));
    }
    // The kw pack doesn't have an entry for 'self', so we offset
    // the output array by one to align them.
    bool add_self = output.size() > _vec.size();
    if (pykwds) {
        if (add_self && !output[0]) {
            // If we haven't set self yet, we're not going to be able to.
            builtin::TypeError::raise(function_name + " takes at least 1 non-keyword argument (0 given)");
        }
        PyObject * key = nullptr;
        PyObject * value = nullptr;
        Py_ssize_t i = 0;
        while (PyDict_Next(pykwds.get(), &i, &key, &value)) {
            std::string key_str = builtin::str(py_ptr::borrow(key));
            bool success = false;
            for (std::size_t n = 0; n < _vec.size(); ++n) {
                if (key_str == _vec[n].name) {
                    if (output[add_self + n]) {
                        builtin::TypeError::raise(
                            function_name + " got multiple values for keyword argument '"
                            + _vec[n].name + "'"
                        );
                    }
                    output[add_self + n] = py_ptr::borrow(value);
                    success = true;
                    break;
                }
            }
            if (!success) {
                builtin::TypeError::raise(
                    function_name + "got an unexpected keyword argument '" + key_str + "'"
                );
            }
        }
    }
    for (std::size_t n = 0; n < _vec.size(); ++n) {
        if (!output[add_self + n] && !_vec[n].value) {
            builtin::TypeError::raise("Missing argument '" + _vec[n].name + "' in call to " + function_name);
        }
    }
}

} // namespace bp3
