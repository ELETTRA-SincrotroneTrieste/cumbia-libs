#ifndef CUTCONTROLSWRITER_H
#define CUTCONTROLSWRITER_H

#include <QString>
#include <vector>
#include <string>

#include <cucontrolswriter_abs.h>
#include <cucontrolsfactories_i.h>

class CumbiaTango;
class CuDataListener;
class CuEpControlsWriterPrivate;
class CuVariant;


class CuTWriterFactory : public CuControlsWriterFactoryI
{
    // CuControlsWriterFactoryI interface
public:
    CuControlsWriterA *create(Cumbia *, CuDataListener *l) const;

    // CuControlsWriterFactoryI interface
public:
    CuControlsWriterFactoryI *clone() const;
};

class CuTControlsWriter: public CuControlsWriterA
{
public:
    CuTControlsWriter(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuTControlsWriter();

    QString targets() const;

    void clearTargets();

    void setTargets(const QString &s);

    void requestProperties(const QStringList &props);

    virtual void execute();

private:
    CuEpControlsWriterPrivate *d;
};

#endif // CUTCONTROLSWRITER_H
