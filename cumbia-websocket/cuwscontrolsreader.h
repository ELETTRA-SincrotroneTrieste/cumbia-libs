#ifndef CUWSCONTROLSREADER_H
#define CUWSCONTROLSREADER_H

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

class CuWsControlsReader : public CuControlsReaderA
{
public:
    CuWsControlsReader(Cumbia *cumbia, CuDataListener *tl);

    virtual ~CuWsControlsReader();

    void setSource(const QString& s);

    QString source() const;

    void unsetSource() ;

    void setOptions(const CuData& o);

    CuData getOptions() const;


private:
    CuWSReaderPrivate *d;

    // CuControlsReaderA interface
public:
    virtual void sendData(const CuData &d);
    virtual void getData(CuData &d_ino) const;
};

#endif // CUWSREADER_H
