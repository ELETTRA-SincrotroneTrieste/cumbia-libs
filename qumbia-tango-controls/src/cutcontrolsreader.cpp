#include "cutcontrolsreader.h"
#include "cutcontrols-utils.h"
#include "cumbiatango.h"
#include "cutangoactionfactories.h"
#include "cutangoactioni.h"
#include "cudatalistener.h"

#include <assert.h>
#include <QCoreApplication>
#include <cumacros.h>

/*! @private
 */
class CuTReaderFactoryPrivate
{
public:
    CuData r_options;
};

/*! the class constructor
 */
CuTReaderFactory::CuTReaderFactory()
{
    d = new CuTReaderFactoryPrivate();
}

/*! \brief the class destructor
 *
 */
CuTReaderFactory::~CuTReaderFactory()
{
    delete d;
}

/*! call this before create to configure the reader
 *
 * @param o a CuData compiled with the following options:
 * \li "period" an integer, in milliseconds, for the polling period. Converted with: CuVariant::toInt
 * \li "refresh_mode": an integer defining the Tango refresh mode. Converted with: CuVariant::toInt
 *
 * \par Important note
 * The options are reset (to an empty CuData) by CuTReaderFactory::create after  CuTControlsReader
 * creation and initialization.
 *
 * \par note
 * Please use the CuTangoOptBuilder class rather than filling in the options manually.
 *
 * \note this method must be called before create in order to transfer the options
 *       to the Tango CuTControlsReader.
 *
 * @see CuTangoOptBuilder
 */
void CuTReaderFactory::setOptions(const CuData &o)
{
    d->r_options = o;
}

/*! \brief returns the options set with setOptions, or an empty CuData.
 *
 * @return CuData with the options set with setOptions, or an empty CuData
 */
CuData CuTReaderFactory::getOptions() const
{
    return d->r_options;
}

/** \brief creates and returns a new CuTControlsReader, configured with the given
 *         Cumbia and CuDataListener.
 *
 * Call setOptions before create to configure the reader.
 *
 * @param c a pointer to the Cumbia object
 * @param l a pointer to an object implementing the CuDataListener interface
 *
 * @return a CuTControlsReader, an implementation of CuControlsReaderA abstract class
 *
 * \par Important note
 * The options are reset (to an empty CuData) by CuTReaderFactory::create after  CuTControlsReader
 * creation and initialization. This avoids applying the same options to subsequent create calls
 * on the same factory
 */
CuControlsReaderA *CuTReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuTControlsReader *r = new CuTControlsReader(c, l);
    r->setOptions(d->r_options);
    d->r_options = CuData(); // reset options
    return r;
}

/** \brief clones the reader factory, allocating a new one with the same options
 *
 * @return a new CuTReaderFactory with options copied from this object
 */
CuControlsReaderFactoryI *CuTReaderFactory::clone() const
{
    CuTReaderFactory *f = new CuTReaderFactory();
    f->d->r_options = d->r_options;
    return f;
}

/*! @private
 */
class CuTControlsReaderPrivate
{
public:
    QString source;
    CumbiaTango *cumbia_tango;
    CuDataListener *tlistener;
    std::vector<std::string> attr_props;
    CuData ta_options;
    bool is_db_op;
};

/*! \brief class constructor that's configured with CumbiaTango and CuDataListener instances
 *
 * @param cumbia_tango a pointer to a CumbiaTango instance
 * @param CuDataListener a pointer to an object implementing the CuDataListener interface
 */
CuTControlsReader::CuTControlsReader(Cumbia *cumbia_tango, CuDataListener *tl)
    : CuControlsReaderA(cumbia_tango, tl)
{
    assert(cumbia_tango->getType() == CumbiaTango::CumbiaTangoType);
    d = new CuTControlsReaderPrivate;
    d->cumbia_tango = static_cast<CumbiaTango *>(cumbia_tango);
    d->tlistener = tl;
    d->is_db_op = false;
}

/*! \brief the class destructor.
 *
 * The class destructors sets the invalidate flag on the listener, so that it won't be
 * used later, and calls unsetSource
 */
CuTControlsReader::~CuTControlsReader()
{
    pdelete("~CuTControlsReader %p", this);
    unsetSource();
    delete d;
}

/*! returns the source of the reader.
 *
 * \par note
 * The returned string will contain the actual source, after wildcards have been
 * replaced.
 *
 * @see setSource
 */
QString CuTControlsReader::source() const
{
    return d->source;
}

/** \brief Stops the reader.
 *
 * This must be called from the main thread.
 *
 * \par Developer note
 *      Calls CumbiaTango::unlinkListener for  CuTangoActionI::ReaderConfig and CuTangoActionI::Reader action types
 *      in a row.
 */
void CuTControlsReader::unsetSource() {
    if(!d->source.isEmpty()) {
        TSource tsrc(d->source.toStdString());
        if(d->is_db_op) {
            d->cumbia_tango->unlinkListener(d->source.toStdString(), CuTangoActionI::TaDb, d->tlistener);
        } else {
            d->cumbia_tango->unlinkListener(d->source.toStdString(), CuTangoActionI::ReaderConfig, d->tlistener);
            d->cumbia_tango->unlinkListener(d->source.toStdString(), CuTangoActionI::Reader, d->tlistener);
        }
        d->source = QString();
    }
}

/*! \brief configures the reader through the options passed as parameter
 *
 * @param o CuData with the options to set on the reader
 *
 * See CuTReaderFactory::setOptions and CuTangoOptBuilder documentation.
 *
 *  This is usually called by CuTReaderFactory::create immediately after CuTControlsWriter is
 * instantiated.
 */
void CuTControlsReader::setOptions(const CuData &o)
{
    d->ta_options = o;
}

/*! \brief forwards the data passed as parameter to the underlying CuTReader
 *
 * \note CuTReader implements the CuTangoActionI interface.
 *
 * This method finds the CuTangoActionI with the source of this reader and calls
 * sendData on it.
 */
void CuTControlsReader::sendData(const CuData &data)
{
    CuTangoActionI *a = d->cumbia_tango->findAction(d->source.toStdString(), CuTangoActionI::Reader);
    if(a)
        a->sendData(data);
}

/*! \brief send a CuData bundle as input argument to the CuTReader associated to this
 *         object with this source and get a reply.
 *
 * @param d_ino a (non const) reference to a CuData compiled with the input arguments
 *        recognised by the CuTReader. When the function returns, the CuData will
 *        contain the reply.
 *
 * \par options recognised by CuTReader.
 * For a list of options recognised by CuTReader, see CuTReader::getData
 *
 * \note CuTReader implements the CuTangoActionI interface.
 *
 * This method finds the CuTangoActionI with the source of this reader and calls
 * getData on it, passing d_ino as argument.
 */
void CuTControlsReader::getData(CuData &d_ino) const
{
    CuTangoActionI *a = d->cumbia_tango->findAction(d->source.toStdString(), CuTangoActionI::Reader);
    if(a)
        a->getData(d_ino);
}

/*! \brief connect the reader to the Tango control system and start reading
 *
 * @param s the name of the source (Tango device attribute or command) for the reader
 *
 * \par Usage
 * Normally, cumbia-qtcontrols widgets provide a setSource method that deals with
 * reader set up through CuContext.
 * If you need to implement a custom reader, please refer to the CuContext documentation
 * and to the examples offered by QuLabel or QuWatcher.
 *
 * \par Source syntax
 * \li test/device/1/double_scalar: read the Tango double_scalar attribute
 * \li inj/diagnostics/cm_inj.01->GetCharge(0,1000): read the Tango command and provide
 *     0, 1000 as input parameters (argin)
 * \li $1->GetCharge(0,1000) same as above, but $1 is replaced by the first device
 *     name found in the command line arguments of the application
 * \li $1->GetCharge(&spinBoxMode, &spinBoxSamples)  same as above, but the two parameters
 *     are sent according to the "value" property of the two spin boxes with the names
 *     specified in the arguments
 *
 * This method starts two activities, one for attribute configuration (i.e. get
 * attribute configuration or get command info) and one for the
 * actual reader. The configuration step is exploited by graphical elements to set
 * minimum, maximum, alarm values, units and so on.
 */
void CuTControlsReader::setSource(const QString &s)
{
    printf("\e[1;32m* \e[0mCuTControlsReader: set source %s on Tango native engine\n", qstoc(s));
    CuTControlsUtils tcu;
    d->source = tcu.replaceWildcards(s, qApp->arguments());
    TSource tsrc(d->source.toStdString());
    d->is_db_op = tsrc.isDbOp();
    if(d->is_db_op) {
        CuTaDbFactory dbf;
        dbf.setOptions(d->ta_options);
        d->cumbia_tango->addAction(tsrc.getName(), d->tlistener, dbf);
    }
    else {
        if(!d->ta_options.value("no-properties").toBool() || d->ta_options.value("single-shot").toBool()) {
            CuTReaderConfFactory acf;
            acf.setOptions(d->ta_options);
            d->cumbia_tango->addAction(d->source.toStdString(), d->tlistener, acf);
        }
        if(!d->ta_options.value("properties-only").toBool() && !d->ta_options.value("single-shot").toBool()) {
            CuTangoReaderFactory readf;
            readf.setOptions(d->ta_options);
            d->cumbia_tango->addAction(d->source.toStdString(), d->tlistener, readf);
        }
    }
}
