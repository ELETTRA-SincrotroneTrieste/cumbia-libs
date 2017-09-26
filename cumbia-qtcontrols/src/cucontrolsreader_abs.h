#ifndef CUCONTROLSREADERA_H
#define CUCONTROLSREADERA_H

#include <string>
#include <vector>
#include <assert.h>

class QString;
class Cumbia;
class CuData;
class CuDataListener;
class QStringList;

class CuControlsReaderA
{
public:
    CuControlsReaderA(Cumbia *c, CuDataListener *l)
    {
        assert(c != NULL && l != NULL);
        m_cumbia = c;
        m_listener = l;
    }

    virtual ~CuControlsReaderA() {}

    virtual void setSource(const QString& s) = 0;

    virtual QString source() const = 0;

    virtual void unsetSource() = 0;

    virtual void requestProperties(const QStringList& props) = 0;

    virtual void sendData(const CuData& d) = 0;

    virtual void getData(CuData& d_ino) const = 0;

    Cumbia *getCumbia() const { return m_cumbia; }

    CuDataListener *getDataListener() const { return m_listener; }

private:
    Cumbia *m_cumbia;
    CuDataListener *m_listener;
};

#endif // CUCONTROLSREADERI_H
