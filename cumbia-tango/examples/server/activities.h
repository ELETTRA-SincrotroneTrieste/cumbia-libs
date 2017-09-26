#ifndef CUTACTIVITY_H
#define CUTACTIVITY_H

#include <cucontinuousactivity.h>
#include <cuisolatedactivity.h>

#include <cuthreadlistener.h>

#include <tango.h>

class CuTActivity : public CuContinuousActivity
{
public:
    CuTActivity(const CuData &token);

    // CuActivity interface
public:
    int getType() const;
    void init();
    void execute();
    void onExit();
    bool matches(const CuData &token) const;

private:
};

class WriteActivity : public CuIsolatedActivity
{
public:
    WriteActivity(const CuData& input);

    ~WriteActivity();

    // CuActivity interface

    void init();
    void execute();
    void onExit();
    bool matches(const CuData &token) const;

    // CuActivity interface
public:
    void event(CuActivityEvent *e);
};

#endif // CUTACTIVITY_H
