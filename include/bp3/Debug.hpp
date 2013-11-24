#ifndef BP3_Debug_hpp_INCLUDED
#define BP3_Debug_hpp_INCLUDED

#ifndef BP3_DEBUG_LOG_LEVEL
#define BP3_DEBUG_LOG_LEVEL 20
#endif

#ifndef BP3_DEBUG_CITIZEN_LEVEL
#define BP3_DEBUG_CITIZEN_LEVEL 20
#endif

#include "bp3/builtin/str.hpp"

#if BP3_DEBUG_LOG_LEVEL >= 0
#include <iostream>
#endif

namespace bp3 {

template <int level, bool enabled=(level <= BP3_DEBUG_LOG_LEVEL)>
struct Log;

template <int level>
struct Log<level,false> {

    static Log & get() {
        static Log instance;
        return instance;
    }

    template <typename T>
    void log(T && v) {}

    template <typename T>
    void log(PyPtr const & ptr) {}

};

template <int level>
struct Log<level,true> {

    static Log & get() {
        static Log instance;
        return instance;
    }

    template <typename T>
    void log(T const & v) {
        std::cerr << v;
    }

    void log(PyPtr const & ptr) {
        std::cerr << std::string(repr(builtin::object(ptr)));
    }

};

template <int level, typename T>
inline Log<level> & operator<<(Log<level> & log, T const & v) {
    log.log(v);
    return log;
}

typedef std::uint64_t CitizenID;

class CitizenBase {
public:

    explicit CitizenBase();

protected:

    friend class CitizenAccess;

    CitizenID _citizen_id;
};

template <int level, bool enabled=(level <= BP3_DEBUG_CITIZEN_LEVEL)>
class Citizen;

template <int level>
class Citizen<level,false> {
public:

    explicit Citizen(std::string const & name) {}

private:
    friend class CitizenAccess;

    CitizenID getCitizenID() const { return 0; }
};

template <int level>
class Citizen<level,true> : public CitizenBase {
public:

    explicit Citizen(std::string const & name);

    ~Citizen();

private:
    friend class CitizenAccess;

    CitizenID getCitizenID() const { return this->_citizen_id; }
};

class CitizenAccess {
public:

    template <int level>
    static CitizenID getID(Citizen<level> const & citizen) { return citizen.getCitizenID(); }


    template <int level>
    static Log<level> & log(Citizen<level> const & citizen) {
        return Log<level>::get() << "Citizen " << getID(citizen) << ": ";
    }

};

template <int level>
inline Citizen<level,true>::Citizen(std::string const & name) : CitizenBase() {
    CitizenAccess::log(*this) << "created with name '" << name << "'\n";
}

template <int level>
inline Citizen<level,true>::~Citizen() {
    CitizenAccess::log(*this) << "destroyed\n";
}

} // namespace bp3

#endif // !BP3_Debug_hpp_INCLUDED
