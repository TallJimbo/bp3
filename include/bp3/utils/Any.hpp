#ifndef BP3_UTILS_Any_hpp_INCLUDED
#define BP3_UTILS_Any_hpp_INCLUDED

#include <memory>

namespace bp3 {
namespace utils {

class Any {
    class ImplBase;
    template <typename T> class ImplT;
public:

    explicit Any() {}

    template <typename T>
    explicit Any(T v);

    Any(Any const &);

    explicit operator bool() const { return !!_impl; }

    template <typename T> T * get();

    template <typename T> T const * get() const;

private:
    std::unique_ptr<ImplBase> _impl;
};

class Any::ImplBase {
public:

    virtual std::unique_ptr<ImplBase> clone() const = 0;

    virtual ~ImplBase() {}

};

template <typename T>
class Any::ImplT : public Any::ImplBase {
public:

    virtual std::unique_ptr<ImplBase> clone() const {
        return std::unique_ptr<ImplBase>(new ImplT(value));
    }

    explicit ImplT(T v) : value(v) {}

    T value;
};

template <typename T>
Any::Any(T v) : _impl(new ImplT<T>(v)) {}

inline Any::Any(Any const & other) : _impl(other._impl ? other._impl->clone() : nullptr) {}

template <typename T>
T * Any::get() {
    if (!_impl) return nullptr;
    ImplT<T> * impl = dynamic_cast<T*>(_impl.get());
    return impl ? &impl->value : nullptr;
}

template <typename T>
T const * Any::get() const {
    if (!_impl) return nullptr;
    ImplT<T> * impl = dynamic_cast<T*>(_impl.get());
    return impl ? &impl->value : nullptr;
}

} // namespace utils
} // namespace bp3

#endif // !BP3_UTILS_Any_hpp_INCLUDED
