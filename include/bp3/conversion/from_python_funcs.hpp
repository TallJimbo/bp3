#ifndef BP3_from_python_funcs_hpp_INCLUDED
#define BP3_from_python_funcs_hpp_INCLUDED

#include "bp3/type_id.hpp"
#include "bp3/py_ptr.hpp"
#include "bp3/conversion/from_python_base.hpp"

namespace bp3 { namespace conversion  {

class from_python_funcs {
public:
    bool is_rvalue;
    from_python_check_func check;
    from_python_convert_func convert;
    from_python_postcall_func postcall;
    from_python_cleanup_func cleanup;
};

}} // namespace bp3::conversion

#endif // !BP3_from_python_funcs_hpp_INCLUDED
