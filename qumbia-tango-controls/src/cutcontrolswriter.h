#ifndef CUTCONTROLSWRITER_H
#define CUTCONTROLSWRITER_H

#include <QString>
#include <vector>
#include <string>

#include <cucontrolswriter_abs.h>
#include <cucontrolsfactories_i.h>

class CumbiaTango;
class CuDataListener;
class CuTangoControlsWriterPrivate;
class CuVariant;
class CuTWriterFactoryPrivate;

class CuTWriterFactory : public CuControlsWriterFactoryI
{
public:
    CuTWriterFactory();

    virtual ~CuTWriterFactory();

    // CuControlsWriterFactoryI interface

    CuControlsWriterA *create(Cumbia *, CuDataListener *l) const;

    CuControlsWriterFactoryI *clone() const;

    void setOptions(const CuData& o);

private:
    CuTWriterFactoryPrivate *d;
};

class CuTControlsWriter: public CuControlsWriterA
{
public:
    CuTControlsWriter(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuTControlsWriter();

    QString target() const;

    void clearTarget();

    void setTarget(const QString &s);

    virtual void execute();

    void sendData(const CuData &d);

    void getData(CuData &d_ino) const;

    void setOptions(const CuData& o);

private:
    CuTangoControlsWriterPrivate *d;
};

#endif // CUTCONTROLSWRITER_H
