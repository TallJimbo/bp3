#include "bp3/Debug.hpp"

namespace bp3 {

namespace {

CitizenID & getNextCitizenID() {
    static CitizenID current = 0;
    return ++current;
}

} // anonymous

CitizenBase::CitizenBase() : _citizen_id(getNextCitizenID()) {}

} // namespace bp3
