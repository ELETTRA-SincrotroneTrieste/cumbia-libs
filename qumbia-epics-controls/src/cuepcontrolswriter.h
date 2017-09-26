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


class CuEpWriterFactory : public CuControlsWriterFactoryI
{
    // CuControlsWriterFactoryI interface
public:
    CuControlsWriterA *create(Cumbia *, CuDataListener *l) const;

    // CuControlsWriterFactoryI interface
public:
    CuControlsWriterFactoryI *clone() const;
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

private:
    CuEpControlsWriterPrivate *d;
};

#endif // CUEPCONTROLSWRITER_H
