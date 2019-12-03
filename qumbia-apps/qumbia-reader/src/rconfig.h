#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QStringList>

class RConfig
{
public:
    RConfig();
    int refresh_limit;
    int truncate, max_timers, verbosity;
    QStringList sources;
    int period;
    bool usage, list_options;
    bool tango_property, property;

    void setTangoProperty();
    void setPropertyOnly();
    QString format;
};

#endif // CONFIGURATION_H
