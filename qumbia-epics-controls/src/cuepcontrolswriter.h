#ifndef CUEPCONTROLSWRITER_H
#define CUEPCONTROLSWRITER_H

#include <QString>
#include <vector>
#include <string>

#include <cucontrolswriter_abs.h>
#include <cucontrolsfactories_i.h>

class CumbiaEpics;
class CuDataListener;
class CuEpControlsWriterPrivate;
class CuVariant;

class CuEpWriterFactoryPrivate;

class CuEpWriterFactory : public CuControlsWriterFactoryI
{
public:
    CuEpWriterFactory();

    virtual ~CuEpWriterFactory();

    CuControlsWriterA *create(Cumbia *, CuDataListener *l) const;

    // CuControlsWriterFactoryI interface
public:
    CuControlsWriterFactoryI *clone() const;

    void setOptions(const CuData &options);

private:
    CuEpWriterFactoryPrivate *d;
};

class CuEpControlsWriter : public CuControlsWriterA
{
public:
    CuEpControlsWriter(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuEpControlsWriter();

    QString targets() const;

    void clearTargets();

    void setTargets(const QString &s);

    void requestProperties(const QStringList &props);

    virtual void execute();

    // CuControlsWriterA interface
public:
    void sendData(const CuData &d);
    void getData(CuData &d_ino) const;

private:
    CuEpControlsWriterPrivate *d;

};

#endif // CUEPCONTROLSWRITER_H
