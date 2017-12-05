#ifndef CUTCONTROLSREADER_H
#define CUTCONTROLSREADER_H

#include <QString>
#include <vector>

#include <cucontrolsreader_abs.h>
#include <cucontrolsfactories_i.h>
#include <cudata.h>

class CuTControlsReaderPrivate;
class CumbiaTango;
class CuDataListener;
class CuTangoOptBuilder;

class CuTReaderFactoryPrivate;

class CuTReaderFactory : public CuControlsReaderFactoryI
{
    // CuControlsWriterFactoryI interface
public:
    CuTReaderFactory();

    virtual ~CuTReaderFactory();

    CuControlsReaderA *create(Cumbia *c, CuDataListener *l) const;

    void setOptions(const CuData& o);

    CuData getOptions() const;

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

private:
    CuTReaderFactoryPrivate *d;
};

class CuTControlsReader : public CuControlsReaderA
{
public:
    CuTControlsReader(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuTControlsReader();

    void setSource(const QString& s);

    QString source() const;

    void unsetSource() ;

    void setOptions(const CuData& o);

    void sendData(const CuData& d);

    void getData(CuData& d_ino) const;

private:
    CuTControlsReaderPrivate *d;
};

#endif // CUTCONTROLSREADERINTERFACE_H
