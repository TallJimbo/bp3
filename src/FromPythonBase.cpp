#include "bp3/Module.hpp"
#include "bp3/FromPythonBase.hpp"
#include "bp3/Registration.hpp"
#include "bp3/Debug.hpp"
#include "bp3/builtin/str.hpp"

namespace bp3 {

namespace {

std::tuple<int,FromPythonFuncs*,ConverterData> findFromPythonConverter(
    std::shared_ptr<Registration> const & reg,
    PyPtr const & py,
    bool is_lvalue,
    int inheritance_offset
) {
    Log<10>::get() << "findFromPythonConverter: starting search for Python object '" << py << "'\n";
    int best_penalty = -1;
    int current_penalty = -1;
    FromPythonFuncs * best_funcs = nullptr;
    ConverterData best_data;
    ConverterData current_data;
    assert(reg);
    for (FromPythonFuncs & current_funcs :  reg->from_python) {
        Log<10>::get() << "findFromPythonConverter: testing converter at " << &current_funcs << "\n";
        if (is_lvalue && !current_funcs.is_lvalue) {
            Log<10>::get() << "findFromPythonConverter: converter at "
                           << &current_funcs << " is not an lvalue\n";
            continue;
        }
        try {
            current_penalty = current_funcs.check(py, current_data);
        } catch (...) {
            Log<10>::get() << "findFromPythonConverter: converter at "
                           << &current_funcs << " check() threw an exception\n";
            current_penalty = -1;
        }
        if (current_penalty < 0) {
            Log<10>::get() << "findFromPythonConverter: converter at "
                           << &current_funcs << " check() failed\n";
            continue;
        }
        // When we have ties between imperfect matches, we choose the first one encountered.
        if (best_funcs && best_penalty <= current_penalty) {
            Log<10>::get() << "findFromPythonConverter: converter at " << &current_funcs
                           << " with penalty=" << current_penalty << ";"
                           << " is not better than " << best_funcs
                           << " with penalty=" << best_penalty << "\n";
            if (current_funcs.cleanup) {
                current_funcs.cleanup(current_data);
                current_data = ConverterData();
            }
        } else {
            Log<10>::get() << "findFromPythonConverter: converter at " << &current_funcs
                           << " with penalty=" << current_penalty << ";"
                           << " is better than " << best_funcs
                           << " with penalty=" << best_penalty << "\n";
            if (best_funcs && best_funcs->cleanup) best_funcs->cleanup(best_data);
            best_funcs = &current_funcs;
            best_data = current_data;
            best_penalty = current_penalty;
            if (best_penalty == 0) {
                Log<10>::get() << "findFromPythonConverter: converter at " << &current_funcs
                               << " is a perfect match\n";
                // perfect match, so we short-circuit and return.
                return std::make_tuple(best_penalty + inheritance_offset, best_funcs, best_data);
            }
        }
        // By now either current_data is the same as best_data, or it's been cleaned up.
    }
    if (best_penalty < 0) {
        // All checks failed (which also means there's nothing to clean up).
        // We don't add inheritance_offset when we fail, of course.
        return std::make_tuple(best_penalty, best_funcs, best_data);
    }
    // If we get here, we have an imperfect match in best_* and current_* is either a duplicate
    // of that or it's already been cleaned up.
    bool have_derived_match = false;
    for (auto iter = reg->derived.begin(); iter != reg->derived.end(); ++iter) {
        // Recurse using derived class; returned penalty will include inheritance_offset
        // unless it's a complete failure.
        FromPythonFuncs * current_funcs = nullptr;
        std::tie(current_penalty,current_funcs,current_data) = findFromPythonConverter(
            iter->second, py, is_lvalue, inheritance_offset + 1
        );
        if (current_penalty < 0) continue;
        if (have_derived_match && best_penalty <= current_penalty) {
            // The new match isn't any better than the old one, and it came from a derived-class sibling.
            if (current_funcs->cleanup) {
                current_funcs->cleanup(current_data);
                current_data = ConverterData(); // reset pointers to null;
            }
        } else {
            if (best_funcs && best_funcs->cleanup) best_funcs->cleanup(best_data);
            best_funcs = current_funcs;
            best_data = current_data;
            best_penalty = current_penalty;
            have_derived_match = true;
            if (best_penalty == 0) {
                // Perfect match, so we short-circuit and return (inheritance offset is already included)
                return std::make_tuple(best_penalty, best_funcs, best_data);
            }
        }
        // By now either current_data is the same as best_data, or it's been cleaned up.
    }
    if (!have_derived_match) best_penalty += inheritance_offset;
    return std::make_tuple(best_penalty, best_funcs, best_data);
}

} // anonymous

FromPythonBase::FromPythonBase(
    Registry const & registry,
    PyPtr const & py,
    bp3::TypeInfo const & ti,
    bool is_lvalue
) : Citizen<10>("FromPythonBase"), _py(py), _penalty(-1), _data(), _funcs(nullptr)
{
    std::shared_ptr<Registration> reg = registry.lookup(ti);
    if (reg) {
        std::tie(_penalty, _funcs, _data) = findFromPythonConverter(reg, py, is_lvalue, 0);
    }
}

void * FromPythonBase::_convert() { return _funcs->convert(_py, _data); }

void FromPythonBase::postcall() { if (_funcs->postcall) _funcs->postcall(_py, _data); }

FromPythonBase::~FromPythonBase() {
    if (_funcs && _funcs->cleanup) _funcs->cleanup(_data);
}

} // namespace bp3
