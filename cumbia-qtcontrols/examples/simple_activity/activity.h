#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <cuisolatedactivity.h>

class Activity : public CuIsolatedActivity
{
public:
    Activity(const CuData &token);

    // CuActivity interface
public:
    int getType() const;
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();
};

#endif // ACTIVITY_H
