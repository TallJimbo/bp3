#include "bp3/keywords.hpp"
#include "bp3/builtin/str.hpp"
#include <cstring>
#include <vector>

namespace bp3 { namespace kw {

void start_pack_parse(
    std::string const & function_name,
    char const ** names, std::size_t const len_names,
    py_ptr const & args, py_ptr const & kwds,
    py_ptr * & output, std::size_t const len_output
) {
    Py_ssize_t const len_args = PyTuple_GET_SIZE(args.get());
    if (static_cast<std::size_t>(len_args) > len_output) {
        builtin::TypeError::raise(
            function_name + " takes at most " + std::to_string(len_output)
            + " non-keyword arguments (" + std::to_string(len_args) + " given)"
        );
    }
    for (Py_ssize_t i = 0; i < len_args; ++i) {
        output[i] = py_ptr::borrow(PyTuple_GET_ITEM(args.get(), i));
    }
    if (kwds) {
        if (len_output > len_names) {
            // The kw pack doesn't have an entry for 'self', so we offset
            // the output array by one to align them.
            // But if we haven't set self yet, we're not going to be able to.
            if (!output[0]) {
                builtin::TypeError::raise(
                    function_name + " takes at least 1 non-keyword argument (0 given)"
                );
            }
            ++output;
        }
        PyObject * key = nullptr;
        PyObject * value = nullptr;
        Py_ssize_t i = 0;
        while (PyDict_Next(kwds.get(), &i, &key, &value)) {
            std::string key_str = builtin::str(py_ptr::borrow(key));
            bool success = false;
            for (std::size_t n = 0; n < len_names; ++n) {
                if (key_str == names[n]) {
                    if (output[n]) {
                        builtin::TypeError::raise(
                            function_name + " got multiple values for keyword argument '" 
                            + std::string(names[n]) + "'"
                        );
                    }
                    output[n] = py_ptr::borrow(value);
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
}

}} // namespace bp3::kw
