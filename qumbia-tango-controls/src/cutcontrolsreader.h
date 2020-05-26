#ifndef CUTCONTROLSREADER_H
#define CUTCONTROLSREADER_H

#include <QString>
#include <vector>

#include <cucontrolsreader_abs.h>
#include <cucontrolsfactories_i.h>
#include <cudata.h>

class CuTControlsReaderPrivate;
class CumbiaTango;
class CuDataListener;
class CuTangoOptBuilder;

class CuTReaderFactoryPrivate;

/** \brief this factory creates Tango readers. Options can be set.
 *
 * Given a pointer to a Cumbia object and a CuDataListener, the create
 * method instantiates and returns a CuTControlsReader, that  implements
 * CuControlsReaderA interface. If setOptions is called before create, the
 * options are set on the CuTControlsReader.
 *
 */
class CuTReaderFactory : public CuControlsReaderFactoryI
{
    // CuControlsWriterFactoryI interface
public:
    CuTReaderFactory();
    virtual ~CuTReaderFactory();

    CuControlsReaderA *create(Cumbia *c, CuDataListener *l) const;

    void setOptions(const CuData& o);
    CuData getOptions() const;

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

private:
    CuTReaderFactoryPrivate *d;
};

/** \brief implements CuControlsReaderA abstract class and provides an interface
 *         to connect a Tango reader to a source.
 *
 * Built with references to CumbiaTango and CuDataListener, CuTControlsReader
 * implements CuControlsReaderA and connects a reader to the Tango control system
 * through the setSource method. setOptions can discretionary be called before setSource.
 *
 * For a list of options recognised by CuTReader, see CuTReader::getData
 *
 * Parameters and configuration can be sent and queried with sendData and getData
 * respectively.
 *
 * unsetSource can finally be called in order to stop a running reader, and if
 * CuTControlsReader is deleted, unsetSource is called automatically.
 *
 */
class CuTControlsReader : public CuControlsReaderA
{
public:
    CuTControlsReader(Cumbia *cumbia_tango, CuDataListener *tl);
    virtual ~CuTControlsReader();

    void setSource(const QString& s);
    QString source() const;
    void unsetSource() ;

    void setOptions(const CuData& o);
    void sendData(const CuData& d);
    void getData(CuData& d_ino) const;

private:
    CuTControlsReaderPrivate *d;
};

#endif // CUTCONTROLSREADERINTERFACE_H
