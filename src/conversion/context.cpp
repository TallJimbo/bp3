#include "bp3/context.hpp"
#include "bp3/conversion/registration.hpp"

#include <cstring>
#include <map>

namespace bp3 {

namespace {

struct compare_type_info {
    bool operator()(bp3::type_info const & a, bp3::type_info const & b) const {
        return std::strcmp(a.name(), b.name()) < 0;
    }
};

typedef std::map<bp3::type_info,std::shared_ptr<conversion::registration>,compare_type_info> registry_map;

} // anonymous

class context_t::context_impl {
public:

    registry_map _registry;

};

std::shared_ptr<conversion::registration> context_t::lookup(bp3::type_info const & t) const {
    registry_map::const_iterator iter = _impl->_registry.find(t);
    std::shared_ptr<conversion::registration> result;
    if (iter != _impl->_registry.end()) result = iter->second;
    return result;
}

void context_t::register_from_python(
    bp3::type_info const & t, bool is_rvalue,
    conversion::from_python_check_func check,
    conversion::from_python_convert_func convert,
    conversion::from_python_postcall_func postcall,
    conversion::from_python_cleanup_func cleanup
) {
    std::shared_ptr<conversion::registration> & reg = _impl->_registry[t];
    if (!reg) {
        reg = std::make_shared<conversion::registration>();
    }
    
    reg->from_python.front()->push_front(
        conversion::from_python_funcs{is_rvalue, check, convert, postcall, cleanup}
    );
}

context_t::context_t() : _impl(new context_impl()) {
    // TODO: add default converters
}

context_t::~context_t() {}

} // namespace bp3
