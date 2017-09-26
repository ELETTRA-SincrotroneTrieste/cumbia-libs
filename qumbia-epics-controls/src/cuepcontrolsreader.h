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

    void setReadOptions(const CuEpicsReadOptions& o);

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

private:
    CuEpReaderFactoryPrivate *d;
};

class CuEpControlsReader : public CuControlsReaderA
{
public:
    CuEpControlsReader(Cumbia *cumbia_epics, CuDataListener *tl);

    virtual ~CuEpControlsReader();

    void setSource(const QString& s);

    QString source() const;

    void unsetSource() ;

    void requestProperties(const QStringList &props);

    void setOptions(const CuEpicsReadOptions& o);

    void sendData(const CuData& d);

    void getData(CuData& d_ino) const;

private:
    CuEpControlsReaderPrivate *d;
};

#endif // CUEPCONTROLSREADER_H
