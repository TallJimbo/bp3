#include "bp3/callables/Callable.hpp"
#include <sstream>
#include <list>

namespace bp3 {
namespace callables {

PyPtr Callable::call(PyPtr const & args, PyPtr const & kwds) const {
    try {
        if (_overloads.size() == static_cast<std::size_t>(1)) {
            OverloadResolutionData data(_overloads.front());
            data.overload->unpackArgs(_name, args, kwds, data, true);
            assert(data.unpack_successful);  // or we should have thrown
            data.overload->convertArgs(_mod, data);
            if (!data.converted_args->score.isValid()) {
                std::ostringstream msg;
                msg << "Error in arguments for '" << _name << "'";
                data.converted_args->reportConversionFailure(msg, "\n  ");
                builtin::TypeError::raise(msg.str());
            }
            // TODO: actually calling the C++ func.
        } else {
            typedef std::list<OverloadResolutionData> DataList;
            DataList data(_overloads.begin(), _overloads.end());
            DataList::iterator i = data.begin();
            while (i != data.end()) {
                i->overload->unpackArgs(_name, args, kwds, *i, false);
                if (i->unpack_successful) {
                    i->overload->convertArgs(_mod, *i);
                    if (i->converted_args->score.isValid()) {
                        ++i;
                    } else {
                        i = data.erase(i);
                    }
                } else {
                    i = data.erase(i);
                }
                // TODO: save non-matches in so we can use them for diagnostics if nothing succeeds
            }
            if (data.empty()) {
                builtin::TypeError::raise("No matching signatures for call to overloaded function '"
                                          + _name + "'");
            }
            DataList::iterator best = data.begin();
            i = data.begin(); ++i;
            while (i != data.end()) {
                int cmp = best->converted_args->score.compare(i->converted_args->score);
                if (cmp > 0) {
                    data.erase(best);
                    best = i;
                    ++i;
                } else if (cmp < 0) {
                    i = data.erase(i);
                }
            }
            if (data.size() != 1) {
                // TODO: diagnostics for ambiguous calls
                builtin::TypeError::raise("Ambiguous call to overloaded function '" + _name + "'");
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
