#ifndef CUEPCONTROLSREADER_H
#define CUEPCONTROLSREADER_H

#include <QString>
#include <vector>

#include <cucontrolsreader_abs.h>
#include <cucontrolsfactories_i.h>
#include <cudata.h>

class CuEpControlsReaderPrivate;
class CumbiaEpics;
class CuDataListener;
class CuEpicsReadOptions;

class CuEpReaderFactoryPrivate;

class CuEpReaderFactory : public CuControlsReaderFactoryI
{
    // CuControlsWriterFactoryI interface
public:
    CuEpReaderFactory();

    virtual ~CuEpReaderFactory();

    CuControlsReaderA *create(Cumbia *c, CuDataListener *l) const;

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

    void setOptions(const CuData &options);

private:
    CuEpReaderFactoryPrivate *d;

public:
};

class CuEpControlsReader : public CuControlsReaderA
{
public:
    CuEpControlsReader(Cumbia *cumbia_epics, CuDataListener *tl);

    virtual ~CuEpControlsReader();

    void setSource(const QString& s);

    QString source() const;

    void unsetSource() ;

    void setOptions(const CuData& o);

    void sendData(const CuData& d);

    void getData(CuData& d_ino) const;

private:
    CuEpControlsReaderPrivate *d;
};

#endif // CUEPCONTROLSREADER_H
