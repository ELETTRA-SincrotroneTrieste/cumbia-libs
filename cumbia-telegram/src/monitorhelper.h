#ifndef MONITORHELPER_H
#define MONITORHELPER_H

#include "botmonitor.h"

class MonitorHelperPrivate;

class MonitorHelper
{
public:
    MonitorHelper();

    void adjustPollers(BotMonitor *mon, int desired_poll_period, int max_avg_period);



private:
    MonitorHelperPrivate *d;
};

#endif // MONITORHELPER_H
