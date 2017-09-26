#ifndef MAINLOOPI_H
#define MAINLOOPI_H

class CuEvent;

class MainLoopEventDeliverer_I
{
public:
    MainLoopEventDeliverer_I();

    virtual void postEvent(CuEvent *e) = 0;
};

#endif // MAINLOOPI_H
