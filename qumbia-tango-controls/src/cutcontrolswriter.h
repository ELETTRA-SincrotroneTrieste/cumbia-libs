#ifndef CUTCONTROLSWRITER_H
#define CUTCONTROLSWRITER_H

#include <QString>
#include <vector>
#include <string>

#include <cucontrolswriter_abs.h>
#include <cucontrolsfactories_i.h>

class CumbiaTango;
class CuDataListener;
class CuTangoControlsWriterPrivate;
class CuVariant;
class CuTWriterFactoryPrivate;

/*! \brief implements CuControlsWriterFactoryI and creates Tango writers
 * Given a pointer to a Cumbia object and a CuDataListener, the create method
 * instantiates and returns a CuTControlsWriter, that implements CuControlsWriterA
 * interface. If setOptions is called before create, the options are set on the
 * CuControlsWriterA.
 *
 * CuTWriterFactory can be cloned by means of the clone method.
 */
class CuTWriterFactory : public CuControlsWriterFactoryI
{
public:
    CuTWriterFactory();

    virtual ~CuTWriterFactory();

    // CuControlsWriterFactoryI interface

    CuControlsWriterA *create(Cumbia *, CuDataListener *l) const;

    CuControlsWriterFactoryI *clone() const;

    void setOptions(const CuData& o);

private:
    CuTWriterFactoryPrivate *d;
};

/** \brief implements CuControlsWriterA abstract class and provides an interface
 *         to connect a Tango writer to a target.
 *
 * Built with references to CumbiaTango and CuDataListener, CuTControlsWriter
 * implements CuControlsWriterA and connects a writer to the Tango control system
 * through the setTarget method. setOptions can discretionary be called before setTarget.
 *
 * Parameters and configuration can be sent and queried with sendData and getData
 * respectively.
 *
 * clearTarget can finally be called in order disconnect a writer, and if
 * CuTControlsWriter is deleted, clearTarget is called automatically.
 *
 */
class CuTControlsWriter: public CuControlsWriterA
{
public:
    CuTControlsWriter(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuTControlsWriter();

    QString target() const;

    void clearTarget();

    void setTarget(const QString &s);

    virtual void execute();

    void sendData(const CuData &d);

    void getData(CuData &d_ino) const;

    void setOptions(const CuData& o);

private:
    CuTangoControlsWriterPrivate *d;
};

#endif // CUTCONTROLSWRITER_H
