#include "bp3/ExceptionAccess.hpp"

namespace bp3 { namespace detail {

ExceptionFactoryBase * ExceptionFactoryBase::search(PyPtr const & target) {
    ExceptionFactoryBase * current = root();
    ExceptionFactoryBase * best = current;
    while (true) {
        MatchEnum match = current->match(target);
        if (match == EXACT) return current;
        if (match == DERIVED) {
            best = current;
            if (current->derived) {
                current = current->derived;
            } else {
                return best;
            }
        } else if (current->sibling) {
            current = current->sibling;
        } else {
            return best;
        }
    }
}


}} // namespace bp3::detail
