#ifndef BP3_context_hpp_INCLUDED
#define BP3_context_hpp_INCLUDED

#include "bp3/type_id.hpp"
#include "bp3/conversion/from_python_base.hpp"

#include <memory>

namespace bp3 {

namespace conversion {

class registration;

} // namespace conversion

class context_t {
public:

    context_t();

    context_t(context_t const &) = delete;

    context_t & operator=(context_t const &) = delete;

    std::shared_ptr<conversion::registration> lookup(bp3::type_info const & t) const;

    void register_from_python(
        bp3::type_info const & t, bool is_rvalue,
        conversion::from_python_check_func check,
        conversion::from_python_convert_func convert,
        conversion::from_python_postcall_func postcall=nullptr,
        conversion::from_python_cleanup_func cleanup=nullptr
    );

    void register_from_python(
        bp3::type_info const & t, bool is_rvalue,
        conversion::from_python_check_func check,
        conversion::from_python_convert_func convert,
        conversion::from_python_cleanup_func cleanup=nullptr
    ) {
        register_from_python(t, is_rvalue, check, convert, nullptr, cleanup);
    }

    ~context_t();

private:

    class context_impl;

    std::unique_ptr<context_impl> _impl;
};

} // namespace bp3

#endif // !BP3_context_hpp_INCLUDED
