#ifndef CUWSREADER_H
#define CUWSREADER_H

#include <QString>
#include <cucontrolsfactories_i.h>
#include <cucontrolsreader_abs.h>

class CuWSReaderFactoryPrivate;
class CuWSReaderPrivate;

class CuWSReaderFactory : public CuControlsReaderFactoryI
{
public:
    CuWSReaderFactory() ;

    virtual ~CuWSReaderFactory();

    CuControlsReaderA *create(Cumbia *c, CuDataListener *l) const;

    virtual void setOptions(const CuData &options);

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

private:
    CuWSReaderFactoryPrivate *d;
};

class CuWSReader : public CuControlsReaderA
{
public:
    CuWSReader(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuWSReader();

    void setSource(const QString& s);

    QString source() const;

    void unsetSource() ;

private:
    CuWSReaderPrivate *d;

    // CuControlsReaderA interface
public:
    virtual void sendData(const CuData &d);
    virtual void getData(CuData &d_ino) const;
};

#endif // CUWSREADER_H
