#include "bp3/keywords.hpp"
#include <cstring>
#include <vector>

namespace bp3 { namespace kw {

bool start_pack_parse(
    char const ** names, std::size_t const len_names,
    py_ptr const & args, py_ptr const & kwds,
    py_ptr * & output, std::size_t const len_output
) {
    Py_ssize_t const len_args = PyTuple_GET_SIZE(args.get());
    if (len_args > len_output) return false;
    for (Py_ssize_t i = 0; i < len_args; ++i) {
        output[i] = py_ptr::borrow(PyTuple_GET_ITEM(args.get(), i));
    }
    if (kwds) {
        if (len_output > len_names) {
            // The kw pack doesn't have an entry for 'self', so we offset
            // the output array by one to align them.
            // But if we haven't set self yet, we're not going to be able to.
            if (!output[0]) return false;
            ++output;
        }
        PyObject * key = nullptr;
        PyObject * value = nullptr;
        Py_ssize_t i = 0;
        while (PyDict_Next(kwds.get(), &i, &key, &value)) {
#if PY_MAJOR_VERSION == 2
            char * key_str = nullptr;
            Py_ssize_t key_len = 0;
            if (PyBytes_AsStringAndSize(key, &key_str, &key_len) != 0) {
                // should never get here - keyword args should always be strings - but throw if we do
                throw_error_already_set();
            }
#endif
            bool success = false;
            for (std::size_t n = 0; n < len_names; ++n) {
                if (
#if PY_MAJOR_VERSION == 2
                    std::strncmp(key_str, names[n], key_len)
#else
                    PyUnicode_CompareWithASCIIString(key, names[n])
#endif
                    == 0
                ) {
                    if (output[n]) {
                        return false; // duplicate arguments
                    }
                    output[n] = py_ptr::borrow(value);
                    success = true;
                    break;
                }
            }
            if (!success) {
                return false;
            }
        }
    }
    return true;
}

}} // namespace bp3::kw
