#ifndef CUHTTPCONTROLSWRITER_H
#define CUHTTPCONTROLSWRITER_H


#include <QString>
#include <vector>
#include <string>

#include <cucontrolswriter_abs.h>
#include <cucontrolsfactories_i.h>

class CumbiaTango;
class CuDataListener;
class CuHttpWriterPrivate;
class CuVariant;
class CuHttpWriterFactoryPrivate;

/*! \brief implements CuControlsWriterFactoryI and creates Http writers
 * Given a pointer to a Cumbia object and a CuDataListener, the create method
 * instantiates and returns a CuHttpWriter, that implements CuControlsWriterA
 * interface. If setOptions is called before create, the options are set on the
 * CuControlsWriterA.
 *
 * CuHttpWriterFactory can be cloned by means of the clone method.
 */
class CuHttpControlsWriterFactory : public CuControlsWriterFactoryI
{
public:
    CuHttpControlsWriterFactory();

    virtual ~CuHttpControlsWriterFactory();

    // CuControlsWriterFactoryI interface

    CuControlsWriterA *create(Cumbia *, CuDataListener *l) const;

    CuControlsWriterFactoryI *clone() const;

    void setOptions(const CuData& o);

private:
    CuHttpWriterFactoryPrivate *d;
};

/** \brief implements CuControlsWriterA abstract class and provides an interface
 *         to connect a Tango writer to a target.
 *
 * Built with references to CumbiaTango and CuDataListener, CuHttpWriter
 * implements CuControlsWriterA and connects a writer to the Tango control system
 * through the setTarget method. setOptions can discretionary be called before setTarget.
 *
 * Parameters and configuration can be sent and queried with sendData and getData
 * respectively.
 *
 * clearTarget can finally be called in order disconnect a writer, and if
 * CuHttpWriter is deleted, clearTarget is called automatically.
 *
 */
class CuHttpControlsWriter: public CuControlsWriterA
{
public:
    CuHttpControlsWriter(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuHttpControlsWriter();

    QString target() const;

    void clearTarget();

    void setTarget(const QString &s);

    virtual void execute();

    void sendData(const CuData &d);

    void getData(CuData &d_ino) const;

    void setOptions(const CuData& o);

private:
    CuHttpWriterPrivate *d;
};

#endif // CUHTTPWRITER_H
