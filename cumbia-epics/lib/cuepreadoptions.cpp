#include "cuepreadoptions.h"


CuEpicsReadOptions::CuEpicsReadOptions()
{
    period = 1000;
    mode = CuMonitor::MonitorRefresh;
}

CuEpicsReadOptions::CuEpicsReadOptions(int per, CuMonitor::RefreshMode mod)
{
    period = per;
    mode = mod;
}
