#include "bp3/Overload.hpp"
#include "bp3/builtin/str.hpp"

namespace bp3 {

OverloadBase::OverloadBase(std::size_t n_args, std::vector<std::string> kwd_names) : _kwd_names(kwd_names) {
    assert(kwd_names.size() == n_args); // TODO: more complete validation, better exception
}

void OverloadBase::unpackArgs(
    std::string const & function_name,
    PyPtr const & pyargs, PyPtr const & pykwds,
    OverloadResolutionData & data, bool throw_on_failure
) const {
    data.unpack_successful = false;
    data.unpacked_args.resize(_kwd_names.size());
    Py_ssize_t const len_pyargs = PyTuple_GET_SIZE(pyargs.get());
    if (static_cast<std::size_t>(len_pyargs) > data.unpacked_args.size()) {
        if (throw_on_failure) {
            builtin::TypeError::raise(
                function_name + " takes at most " + std::to_string(data.unpacked_args.size())
                + " non-keyword arguments (" + std::to_string(len_pyargs) + " given)"
            );
        } else {
            return;
        }
    }
    for (Py_ssize_t i = 0; i < len_pyargs; ++i) {
        data.unpacked_args[i] = PyPtr::borrow(PyTuple_GET_ITEM(pyargs.get(), i));
    }
    if (pykwds) {
        PyObject * key = nullptr;
        PyObject * value = nullptr;
        Py_ssize_t i = 0;
        while (PyDict_Next(pykwds.get(), &i, &key, &value)) {
            std::string key_str = builtin::str(PyPtr::borrow(key));
            bool success = false;
            for (std::size_t n = 0; n < _kwd_names.size(); ++n) {
                if (key_str == _kwd_names[n]) {
                    if (data.unpacked_args[n]) {
                        if (throw_on_failure) {
                            builtin::TypeError::raise(
                                function_name + " got multiple values for keyword argument '"
                                + _kwd_names[n] + "'"
                            );
                        } else {
                            return;
                        }
                    }
                    data.unpacked_args[n] = PyPtr::borrow(value);
                    success = true;
                    break;
                }
            }
            if (!success) {
                if (throw_on_failure) {
                    builtin::TypeError::raise(
                        function_name + " got an unexpected keyword argument '" + key_str + "'"
                    );
                } else {
                    return;
                }
            }
        }
    }
    for (std::size_t n = 0; n < _kwd_names.size(); ++n) {
        if (!data.unpacked_args[n]) {
            if (throw_on_failure) {
                builtin::TypeError::raise(
                    "Missing argument '" + _kwd_names[n] + "' in call to " + function_name
                );
            } else {
                return;
            }
        }
    }
    data.unpack_successful = true;
}

} // namespace bp3
