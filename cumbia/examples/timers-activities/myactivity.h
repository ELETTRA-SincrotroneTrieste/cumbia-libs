#ifndef MYACTIVITY
#define MYACTIVITY

#include <cudatalistener.h>
#include <cuperiodicactivity.h>
#include <cuthreadtokengeni.h>

// activity data
class ThreadTokGen : public CuThreadTokenGenI {
public:
    ThreadTokGen(int limit);

    // CuThreadTokenGenI interface
public:
    std::string generate(const std::string &in);

private:
    int m_limit;
};

class MyActivity : public CuPeriodicActivity
{
public:
    MyActivity(int idx, bool verbose, int sleep_ms);

    // CuActivity interface
public:
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    unsigned m_step;
    unsigned m_idx; // index of activity
    bool m_verbose;
    int m_sleep_ms;
};

#endif // MYACTIVITYLISTENER_H
