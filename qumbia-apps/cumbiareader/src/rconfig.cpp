#include "rconfig.h"
#include "cumbiareader.h"

RConfig::RConfig()
{
    refresh_limit = -1;
    verbosity = Cumbiareader::Low;
    truncate = -1;
    max_timers = -1;
    usage = list_options = false;
    period = 1000;
    property = false;
    tango_property = false;
    format = QString();
}

void RConfig::setTangoProperty()
{
    tango_property = true;
    refresh_limit = 1;
}

void RConfig::setPropertyOnly()
{
    property = true;
    refresh_limit = 1;
}
