#ifndef CUHTTPCONTROLSREADER_H
#define CUHTTPCONTROLSREADER_H

#include <QString>
#include <cucontrolsfactories_i.h>
#include <cucontrolsreader_abs.h>

class CuHTTPReaderFactoryPrivate;
class CuHTTPReaderPrivate;

class CuHTTPReaderFactory : public CuControlsReaderFactoryI
{
public:
    CuHTTPReaderFactory() ;

    virtual ~CuHTTPReaderFactory();

    CuControlsReaderA *create(Cumbia *c, CuDataListener *l) const;

    virtual void setOptions(const CuData &options);

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

private:
    CuHTTPReaderFactoryPrivate *d;
};

class CuHttpControlsReader : public CuControlsReaderA
{
public:
    CuHttpControlsReader(Cumbia *cumbia, CuDataListener *tl);

    virtual ~CuHttpControlsReader();

    void setSource(const QString& s);

    QString source() const;

    void unsetSource() ;

    void setOptions(const CuData& o);

    CuData getOptions() const;


private:
    CuHTTPReaderPrivate *d;

    // CuControlsReaderA interface
public:
    virtual void sendData(const CuData &d);
    virtual void getData(CuData &d_ino) const;
};

#endif // CUHTTPREADER_H
