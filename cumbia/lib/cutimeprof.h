#ifndef CUTIMEPROF_H
#define CUTIMEPROF_H

#include <chrono>
#include <string>
#include <map>

class CuTimeProfP;

class CuTimeProf
{
public:
    enum Unit { Ms, Us };

    CuTimeProf(bool add_now = true, const char* tag = nullptr);
    ~CuTimeProf();
    void add(const char* tag);

    void print(const char *msg, Unit u, const char* tag = nullptr);

    std::map<std::chrono::high_resolution_clock::time_point, std::string> results() const;

private:
    CuTimeProfP *d;
};

#endif // CUTIMEPROF_H
