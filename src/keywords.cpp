#include "bp3/keywords.hpp"
#include "bp3/builtin/str.hpp"
#include <cstring>
#include <vector>

namespace bp3 {

/*
 * For overloaded functions/methods, throw_on_failure is false, and we
 * just rely on the return value to indicate success (true) or failure
 * (false), and hence avoid try/catch inefficiencies when using
 * keyword arguments to aid with overload resolution.
 *
 * For non-overloaded functions, throw_on_failure is true, so we can
 * raise more desciptive exceptions within this routine.
 */
bool arg_def_list::parse(
    std::string const & function_name,
    py_ptr const & pyargs, py_ptr const & pykwds,
    std::vector<py_ptr> & output,
    bool throw_on_failure
) const {
    output.resize(_vec.size());
    Py_ssize_t const len_pyargs = PyTuple_GET_SIZE(pyargs.get());
    if (static_cast<std::size_t>(len_pyargs) > output.size()) {
        if (throw_on_failure) {
            builtin::TypeError::raise(
                function_name + " takes at most " + std::to_string(output.size())
                + " non-keyword arguments (" + std::to_string(len_pyargs) + " given)"
            );
        } else {
            return false;
        }
    }
    for (Py_ssize_t i = 0; i < len_pyargs; ++i) {
        output[i] = py_ptr::borrow(PyTuple_GET_ITEM(pyargs.get(), i));
    }
    if (pykwds) {
        PyObject * key = nullptr;
        PyObject * value = nullptr;
        Py_ssize_t i = 0;
        while (PyDict_Next(pykwds.get(), &i, &key, &value)) {
            std::string key_str = builtin::str(py_ptr::borrow(key));
            bool success = false;
            for (std::size_t n = 0; n < _vec.size(); ++n) {
                if (key_str == _vec[n].name) {
                    if (output[n]) {
                        if (throw_on_failure) {
                            builtin::TypeError::raise(
                                function_name + " got multiple values for keyword argument '"
                                + _vec[n].name + "'"
                            );
                        } else {
                            return false;
                        }
                    }
                    output[n] = py_ptr::borrow(value);
                    success = true;
                    break;
                }
            }
            if (!success) {
                if (throw_on_failure) {
                    builtin::TypeError::raise(
                        function_name + "got an unexpected keyword argument '" + key_str + "'"
                    );
                } else {
                    return false;
                }
            }
        }
    }
    for (std::size_t n = 0; n < _vec.size(); ++n) {
        if (!output[n] && !_vec[n].value) {
            if (throw_on_failure) {
                builtin::TypeError::raise(
                    "Missing argument '" + _vec[n].name + "' in call to " + function_name
                );
            } else {
                return false;
            }
        }
    }
    return true;
}

void arg_def_list::validate(std::size_t n_args) {
    if (n_args > _vec.size()) {
        _vec.insert(_vec.begin(), arg("self"));
    }
    // TODO: throw if sizes still aren't correct, check for duplicate keywords
    //       and defaults that aren't at the end.
}

} // namespace bp3
