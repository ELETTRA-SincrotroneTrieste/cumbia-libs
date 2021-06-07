#include "rconfig.h"
#include "qumbia-reader.h"

RConfig::RConfig()
{
    refresh_limit = 1; // 0 means monitor
    verbosity = QumbiaReader::Low;
    truncate = -1;
    max_timers = -1;
    usage = list_options = false;
    period = 1000;
    format = QString();
    property = help = no_properties = false;
}


void RConfig::setPropertyOnly()
{
    property = true;
    refresh_limit = 1;
}
