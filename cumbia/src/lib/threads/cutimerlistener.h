#ifndef CUTIMERLISTENER_H
#define CUTIMERLISTENER_H

class CuTimer;

class CuTimerListener
{
public:
    virtual void onTimeout(CuTimer*) = 0;
};

#endif // CUTIMERLISTENER_H
