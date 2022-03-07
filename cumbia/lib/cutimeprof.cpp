#include "cutimeprof.h"

class CuTimeProfP {
public:
    std::map<std::chrono::high_resolution_clock::time_point, std::string> m;
};

CuTimeProf::CuTimeProf(bool add_now, const char *tag) {
    d = new CuTimeProfP;
    if(add_now)
        d->m[std::chrono::high_resolution_clock::now()] = tag != nullptr ? std::string(tag) : "start";
}

CuTimeProf::~CuTimeProf() {
    delete d;
}

void CuTimeProf::add(const char *tag) {
    d->m[std::chrono::high_resolution_clock::now()] = std::string(tag);
}

/*! \brief print the accumulated output
 *  \param msg optional message
 *  \param u unit either CuTimeProf::Ms or CuTimeProf::Us
 *  \param tag if not null, print the duration from the previous entry to tag
 */
void CuTimeProf::print(const char *msg, Unit u, const char *tag) {
    if(tag != nullptr) {
        int i = 0;
        for(std::map<std::chrono::high_resolution_clock::time_point, std::string>::const_iterator it = d->m.begin(); it != d->m.end(); ++it) {

            if(std::string(tag) == it->second && i > 0) {
                std::map<std::chrono::high_resolution_clock::time_point, std::string>::const_iterator itprev = std::prev(it, 1);
                std::chrono::high_resolution_clock::time_point prev = itprev->first;

            }

    }
}

std::map<std::chrono::system_clock::time_point, std::string> CuTimeProf::results() const {
    return d->m;
}


