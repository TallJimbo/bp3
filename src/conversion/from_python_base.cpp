#include "bp3/module.hpp"
#include "bp3/conversion/from_python_base.hpp"
#include "bp3/conversion/from_python_funcs.hpp"
#include "bp3/conversion/registration.hpp"

namespace bp3 { namespace conversion {

namespace {

std::tuple<int,from_python_funcs*,converter_data> find_from_python_converter(
    std::shared_ptr<registration> const & reg,
    PyPtr const & py,
    bool is_lvalue,
    int inheritance_offset
) {
    int best_penalty = -1;
    int current_penalty = -1;
    from_python_funcs * best_funcs = nullptr;
    converter_data best_data;
    converter_data current_data;
    assert(reg);
    for (auto converter_list : reg->from_python) {
        for (from_python_funcs & current_funcs : *converter_list) {
            if (is_lvalue && !current_funcs.is_lvalue) {
                continue;
            }
            try {
                current_penalty = current_funcs.check(py, current_data);
            } catch (...) {
                current_penalty = -1;
            }
            if (current_penalty < 0) {
                continue;
            }
            // When we have ties between imperfect matches, we choose the first one encountered.
            if (best_funcs && best_penalty <= current_penalty) {
                if (current_funcs.cleanup) current_funcs.cleanup(current_data);
            } else {
                if (best_funcs && best_funcs->cleanup) best_funcs->cleanup(best_data);
                best_funcs = &current_funcs;
                best_data = current_data;
                best_penalty = current_penalty;
                if (best_penalty == 0) {
                    // perfect match, so we short-circuit and return.
                    return std::make_tuple(best_penalty + inheritance_offset, best_funcs, best_data);
                }
            }
            // By now either current_data is the same as best_data, or it's been cleaned up.
        }
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
        from_python_funcs * current_funcs = nullptr;
        std::tie(current_penalty,current_funcs,current_data) = find_from_python_converter(
            *iter, py, is_lvalue, inheritance_offset + 1
        );
        if (current_penalty < 0) continue;
        if (have_derived_match && best_penalty <= current_penalty) {
            // The new match isn't any better than the old one, and it came from a derived-class sibling.
            if (current_funcs->cleanup) current_funcs->cleanup(current_data);
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

from_python_base::from_python_base(
    module const & mod,
    PyPtr const & py,
    bp3::TypeInfo const & ti,
    bool is_lvalue
) : _py(py), _penalty(-1), _data(), _funcs(nullptr)
{
    std::shared_ptr<registration> reg = mod.lookup(ti);
    if (reg) {
        std::tie(_penalty, _funcs, _data) = find_from_python_converter(reg, py, is_lvalue, 0);
    }
}

void * from_python_base::_convert() { return _funcs->convert(_py, _data); }

void from_python_base::postcall() { if (_funcs->postcall) _funcs->postcall(_py, _data); }

from_python_base::~from_python_base() {
    if (_funcs && _funcs->cleanup) _funcs->cleanup(_data);
}

}} // namespace bp3::conversion
