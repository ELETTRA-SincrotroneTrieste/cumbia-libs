#ifndef CURNDREADER_H
#define CURNDREADER_H

#include <QString>
#include <cucontrolsfactories_i.h>
#include <cucontrolsreader_abs.h>

class CuRNDReaderFactoryPrivate;
class CuRNDReaderPrivate;

class CuRNDReaderFactory : public CuControlsReaderFactoryI
{
public:
    CuRNDReaderFactory() ;

    virtual ~CuRNDReaderFactory();

    CuControlsReaderA *create(Cumbia *c, CuDataListener *l) const;

    virtual void setOptions(const CuData &options);

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

private:
    CuRNDReaderFactoryPrivate *d;
};

class CuRNDReader : public CuControlsReaderA
{
public:
    CuRNDReader(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuRNDReader();

    void setSource(const QString& s);

    QString source() const;

    void unsetSource() ;

    void setOptions(const CuData& o);

    CuData getOptions() const;

private:
    CuRNDReaderPrivate *d;

    // CuControlsReaderA interface
public:
    virtual void sendData(const CuData &d);
    virtual void getData(CuData &d_ino) const;
};

#endif // CURNDREADER_H
