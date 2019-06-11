#include "cutcontrolswriter.h"
#include "cutcontrols-utils.h"

#include <cumbiatango.h>
#include <cudatalistener.h>
#include <cutangoactionfactories.h>
#include <QCoreApplication>
#include <cumacros.h>
#include <assert.h>

/*! @private
 */
class CuTWriterFactoryPrivate
{
public:
    CuData w_options;
};

/*! the class constructor
 */
CuTWriterFactory::CuTWriterFactory()
{
    d = new CuTWriterFactoryPrivate;
}

/*! \brief the class destructor
 *
 */
CuTWriterFactory::~CuTWriterFactory()
{
    delete d;
}

/*! \brief set options through a CuData bundle. They are then passed to the
 *         CuTControlsWriter instantiated and returned by the create method
 *
 * @param o a CuData bundle of key/value pairs
 *
 * \par Key/value pairs
 * \li *no-properties* [bool, default: false]. If true, skip attribute configuration in setTarget.
 * \li *fetch_props* [std::vector<std::string>, default: empty] list of attribute properties to
 *     fetch from the database during configuration. The *no-properties* key must be left to false
 *     (default).
 *
 * @see create
 */
void CuTWriterFactory::setOptions(const CuData &o)
{
    d->w_options = o;
}

/** \brief creates and returns a new CuTControlsWriter, configured with the given
 *         Cumbia and CuDataListener.
 *
 * Call setOptions before create to configure the writer.
 * See setOptions for a list of predefined keys.
 *
 * @param c a pointer to the CumbiaTango object
 * @param l a pointer to an object implementing the CuDataListener interface
 *
 * @return a CuTControlsWriter, an implementation of CuControlsWriterA abstract class
 */
CuControlsWriterA *CuTWriterFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuTControlsWriter *wf = new CuTControlsWriter(c, l);
    wf->setOptions(d->w_options);
    return wf;
}

/** \brief clones the writer factory, allocating a new one with the same options
 *
 * @return a new CuTWriterFactory with options copied from this object
 */
CuControlsWriterFactoryI *CuTWriterFactory::clone() const
{
    CuTWriterFactory *wf = new CuTWriterFactory();
    wf->d->w_options = d->w_options;
    return wf;
}

/*! @private
 */
class CuTangoControlsWriterPrivate
{
public:
    QString target;
    CumbiaTango *cumbia_tango;
    CuDataListener *tlistener;
    CuData w_options;
};

/*! \brief class constructor that's configured with CumbiaTango and CuDataListener instances
 *
 * @param cumbia_tango a pointer to a CumbiaTango instance
 * @param CuDataListener a pointer to an object implementing the CuDataListener interface
 */
CuTControlsWriter::CuTControlsWriter(Cumbia *cumbia_tango, CuDataListener *tl)
 : CuControlsWriterA(cumbia_tango, tl)
{
    assert(cumbia_tango->getType() == CumbiaTango::CumbiaTangoType);
    d = new CuTangoControlsWriterPrivate;
    d->cumbia_tango = static_cast<CumbiaTango *>(cumbia_tango);
    d->tlistener = tl;
}

/*! \brief the class destructor.
 *
 * The class destructors sets the invalidate flag on the listener, so that it won't be
 * used later, and calls clearTarget
 */
CuTControlsWriter::~CuTControlsWriter()
{
    d->tlistener->invalidate();
    clearTarget();
    delete d;
}

/*! \brief set the options on the writer (before setTarget)
 *
 * This is usually called by CuTWriterFactory::create immediately after CuTControlsWriter is
 * instantiated.
 */
void CuTControlsWriter::setOptions(const CuData &o)
{
    d->w_options = o;
}

/*! \brief returns the configured target, or an empty string
 *
 * @return the configured target or an empty string if setTarget hasn't been
 * called yet.
 *
 * \par note
 * The returned string will contain the actual target, after wildcards have been
 * replaced.
 */
QString CuTControlsWriter::target() const
{
    return d->target;
}

/*! \brief remove the configured target, if any.
 *
 * \li unlinks the attribute configuration listener
 * \li unlinks the writer listener
 *
 */
void CuTControlsWriter::clearTarget()
{
    d->cumbia_tango->unlinkListener(d->target.toStdString(), CuTangoActionI::WriterConfig, d->tlistener);
    d->cumbia_tango->unlinkListener(d->target.toStdString(), CuTangoActionI::Writer, d->tlistener);
    d->target = QString();
}

/*! \brief execute the target configured with setTarget. The value to write is taken
 *         from the return value of getArgs.
 *
 * A CuTangoWriterFactory is used to start an activity managed by CumbiaTango.
 * The activity deals with writing the value on an attribute or imparting a
 * command once.
 *
 */
void CuTControlsWriter::execute()
{
    CuTangoWriterFactory wtf;
    wtf.setWriteValue(getArgs());
    wtf.setConfiguration(getConfiguration());
    d->cumbia_tango->addAction(d->target.toStdString(), d->tlistener, wtf);
}

/*! \brief This is not implemented yet
 */
void CuTControlsWriter::sendData(const CuData & /* data */)
{
    printf("CuTControlsWriter.sendData: not implemented\n");
}

/*! \brief This is not implemented yet
 */
void CuTControlsWriter::getData(CuData & /*d_ino*/) const
{
    printf("CuTControlsWriter.getData: not implemented\n");
}

/*! \brief connect the writer to the Tango control system target
 *
 * @param s the name of the writer target (Tango device attribute or command)
 *
 * \par Usage
 * Normally, cumbia-qtcontrols widgets provide a setTarget method that deals with
 * writer set up through CuContext.
 * If you need to implement a custom writer, please refer to the CuContext documentation
 * and to the examples offered by QuButton, QuApplyNumeric or QuWriter.
 *
 * \par Target syntax
 * \li test/device/1/double_scalar(100.0): write 100.0 on the Tango double_scalar attribute
 * \li test/device/1/double_scalar: connect and prepare for later writing
 * \li inj/diagnostics/cm_inj.01->AcquireBackground: connect and prepare for imparting a command later on
 * \li $1->AcquireBackground same as above, but $1 is replaced by the first device
 *     name found in the command line arguments of the application
 * \li $1/double_scalar(&doubleSpinBox)  same as above, but the parameter
 *     is sent according to the "value" property of the spin box with the name
 *     specified
 *
 * This method starts one activity to configure the target (i.e. get attribute
 * configuration or get command info). The configuration step is exploited by
 * graphical elements to set minimum, maximum, alarm values, units and so on.
 */
void CuTControlsWriter::setTarget(const QString &s)
{
    CuTControlsUtils tcu;
    d->target = tcu.replaceWildcards(s, qApp->arguments());
    if(!d->w_options["no-properties"].toBool()) {
        CuTWriterConfFactory w_conf_factory;
        w_conf_factory.setOptions(d->w_options);
        d->cumbia_tango->addAction(d->target.toStdString(), d->tlistener, w_conf_factory);
    }
}
