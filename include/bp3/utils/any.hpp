#ifndef BP3_UTILS_any_hpp_INCLUDED
#define BP3_UTILS_any_hpp_INCLUDED

#include <memory>

namespace bp3 {
namespace utils {

class any {
    class impl_base;
    template <typename T> class impl_t;
public:

    explicit any() {}

    template <typename T>
    explicit any(T v);

    any(any const &);

    explicit operator bool() const { return !!_impl; }

    template <typename T> T * get();

    template <typename T> T const * get() const;

private:
    std::unique_ptr<impl_base> _impl;
};

class any::impl_base {
public:

    virtual std::unique_ptr<impl_base> clone() const = 0;

    virtual ~impl_base() {}

};

template <typename T>
class any::impl_t : public any::impl_base {
public:

    virtual std::unique_ptr<impl_base> clone() const {
        return std::unique_ptr<impl_base>(new impl_t(value));
    }

    explicit impl_t(T v) : value(v) {}

    T value;
};

template <typename T>
any::any(T v) : _impl(new impl_t<T>(v)) {}

inline any::any(any const & other) : _impl(other._impl ? other._impl->clone() : nullptr) {}

template <typename T>
T * any::get() {
    if (!_impl) return nullptr;
    impl_t<T> * impl = dynamic_cast<T*>(_impl.get());
    return impl ? &impl->value : nullptr;
}

template <typename T>
T const * any::get() const {
    if (!_impl) return nullptr;
    impl_t<T> * impl = dynamic_cast<T*>(_impl.get());
    return impl ? &impl->value : nullptr;
}

} // namespace utils
} // namespace bp3

#endif // !BP3_UTILS_any_hpp_INCLUDED
