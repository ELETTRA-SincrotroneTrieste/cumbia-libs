#ifndef RNDSOURCEBUILDER_H
#define RNDSOURCEBUILDER_H

#include <QString>
#include <cudata.h>

class RndSourceBuilder
{
public:
    RndSourceBuilder();
    RndSourceBuilder(const QString& name, int size = 1, double min = 0, double max = 1000, int period = 1000, const QString &label = QString());
    void fromSource(const QString& src);

    void setFGenFromFile(const QString& filename);

    QString name;
    CuData options;
};

#endif // RNDSOURCEBUILDER_H
