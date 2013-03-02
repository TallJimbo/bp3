#ifndef BP3_CONVERSION_args_from_python_hpp_INCLUDED
#define BP3_CONVERSION_args_from_python_hpp_INCLUDED

#include "bp3/conversion/from_python.hpp"
#include "bp3/meta.hpp"

namespace bp3 { namespace conversion  {

namespace detail {

template <std::size_t N, typename ...E>
struct args_from_python_impl;

template <std::size_t N>
struct args_from_python_impl {};

template <std::size_t N, typename T, typename ...E>
struct args_from_python_impl : public args_from_python_impl<N+1,E...> {
    
    typedef args_from_python_impl<N+1,E...> base_t;

    explicit args_from_python_impl(context_t const & context, std::vector<py_ptr> const & py_args) :
        base_t(context, py_args), _elem(py_args[N])
    {}

    args_from_python_impl(args_from_python_impl const &) = delete;
    args_from_python_impl & operator=(args_from_python_impl const &) = delete;

    int penalty() const {
        return _elem.penalty() + base_t::penalty();
    }
    
    from_python<T> _elem;
};

} // namespace detail


}} // namespace bp3::conversion

#endif // !BP3_CONVERSION_args_from_python_hpp_INCLUDED
