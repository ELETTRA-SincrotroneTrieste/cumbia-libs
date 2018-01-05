#ifndef GETTDBPROPACTIVITY_H
#define GETTDBPROPACTIVITY_H

#include <cuactivity.h>
#include <list>

class CuGetTDbPropActivityPrivate;

class CuGetTDbPropActivity : public CuActivity
{
public:
    CuGetTDbPropActivity(const std::list<CuData> &in_data);

    virtual ~CuGetTDbPropActivity();

    // CuActivity interface
public:
    int getType() const;
    void event(CuActivityEvent *e);
    bool matches(const CuData &token) const;
    int repeat() const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuGetTDbPropActivityPrivate *d;
};

#endif // GETTDBPROPACTIVITY_H
