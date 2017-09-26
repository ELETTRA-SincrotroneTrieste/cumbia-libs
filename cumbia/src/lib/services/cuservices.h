#ifndef SERVICES_H
#define SERVICES_H


class CuServices
{
public:
    CuServices() {}

    enum Type { Thread = 0, ActivityManager, EventLoop, Log, User = 100 };
};

#endif // SERVICES_H
