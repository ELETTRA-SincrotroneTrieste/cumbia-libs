#ifndef CUWSCONTROLSWRITER_H
#define CUWSCONTROLSWRITER_H


#include <QString>
#include <vector>
#include <string>

#include <cucontrolswriter_abs.h>
#include <cucontrolsfactories_i.h>

class CumbiaTango;
class CuDataListener;
class CuWsWriterPrivate;
class CuVariant;
class CuWsWriterFactoryPrivate;

/*! \brief implements CuControlsWriterFactoryI and creates Websocket writers
 * Given a pointer to a Cumbia object and a CuDataListener, the create method
 * instantiates and returns a CuWsWriter, that implements CuControlsWriterA
 * interface. If setOptions is called before create, the options are set on the
 * CuControlsWriterA.
 *
 * CuWsWriterFactory can be cloned by means of the clone method.
 */
class CuWsControlsWriterFactory : public CuControlsWriterFactoryI
{
public:
    CuWsControlsWriterFactory();

    virtual ~CuWsControlsWriterFactory();

    // CuControlsWriterFactoryI interface

    CuControlsWriterA *create(Cumbia *, CuDataListener *l) const;

    CuControlsWriterFactoryI *clone() const;

    void setOptions(const CuData& o);

private:
    CuWsWriterFactoryPrivate *d;
};

/** \brief implements CuControlsWriterA abstract class and provides an interface
 *         to connect a Tango writer to a target.
 *
 * Built with references to CumbiaTango and CuDataListener, CuWsWriter
 * implements CuControlsWriterA and connects a writer to the Tango control system
 * through the setTarget method. setOptions can discretionary be called before setTarget.
 *
 * Parameters and configuration can be sent and queried with sendData and getData
 * respectively.
 *
 * clearTarget can finally be called in order disconnect a writer, and if
 * CuWsWriter is deleted, clearTarget is called automatically.
 *
 */
class CuWsControlsWriter: public CuControlsWriterA
{
public:
    CuWsControlsWriter(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuWsControlsWriter();

    QString target() const;

    void clearTarget();

    void setTarget(const QString &s);

    virtual void execute();

    void sendData(const CuData &d);

    void getData(CuData &d_ino) const;

    void setOptions(const CuData& o);

private:
    CuWsWriterPrivate *d;
};

#endif // CUWSWRITER_H
