#ifndef CUEVENTLOOP_H
#define CUEVENTLOOP_H

#include <stdlib.h>
#include <cuservicei.h>

class CuData;
class CuEventI;
class CuEventLoopPrivate;
class CuELoopEventPrivate;
class CuEventI;

class CuEventLoopListener
{
public:
    virtual void onEvent(CuEventI *e) = 0;
};

class CuEventLoopService : public CuServiceI
{
public:

public:
    CuEventLoopService(CuEventLoopListener *l = NULL);

    virtual ~CuEventLoopService();

    void exec(bool threaded);

    void postEvent(CuEventI* e);

    void setCuEventLoopListener(CuEventLoopListener *l);

    void exit();

    void wait();

protected:
    virtual void run();

private:
    CuEventLoopPrivate *d;

    // CuServiceI interface
public:
    std::string getName() const;

    CuServices::Type getType() const;
};

#endif // CUEVENTLOOP_H
