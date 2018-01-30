#ifndef CUCONTROLSWRITERA_H
#define CUCONTROLSWRITERA_H

#include <string>
#include <vector>
#include <assert.h>
#include <cuvariant.h>

class QString;
class Cumbia;
class CuData;
class CuDataListener;
class QStringList;

class CuControlsWriterAbstractPrivate
{
public:
    Cumbia* cumbia;
    CuDataListener *listener;
    CuVariant argins;
};

class CuControlsWriterA
{
public:
    CuControlsWriterA(Cumbia *c, CuDataListener *l)
    {
        assert(c != NULL && l != NULL);
        d = new CuControlsWriterAbstractPrivate;
        d->cumbia = c;
        d->listener = l;
    }

    virtual ~CuControlsWriterA()
    {
        delete d;
    }

    virtual QString target() const = 0;

    virtual void clearTargets() = 0;

    virtual void setTarget(const QString &s) = 0;

    virtual void execute() = 0;

    virtual void sendData(const CuData& d) = 0;

    virtual void getData(CuData& d_ino) const = 0;

    CuVariant getArgs() const { return d->argins; }

    void setArgs(const CuVariant &args) { d->argins = args; }

    Cumbia *getCumbia() const { return d->cumbia; }

    CuDataListener *getDataListener() const { return d->listener; }

private:
    CuControlsWriterAbstractPrivate *d;
};

#endif // CUCONTROLSWRITERI_H
