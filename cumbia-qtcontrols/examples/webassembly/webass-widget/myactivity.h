#ifndef MYACTIVITY_H
#define MYACTIVITY_H

#include <cucontinuousactivity.h>

class MyActivity : public CuContinuousActivity
{
public:
    MyActivity();

    // CuActivity interface
public:
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    int m_cnt;
};

#endif // MYACTIVITY_H
