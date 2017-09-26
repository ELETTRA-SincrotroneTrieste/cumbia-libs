#ifndef CUEPICSREADOPTIONS_H
#define CUEPICSREADOPTIONS_H

#include <cumonitor.h>

class CuEpicsReadOptions
{
public:
    CuEpicsReadOptions();

    CuEpicsReadOptions(int per, CuMonitor::RefreshMode mod);

    int period;
    CuMonitor::RefreshMode mode;
};

#endif // CUTREADOPTIONS_H
