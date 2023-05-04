#ifndef QuWatcher_H
#define QuWatcher_H

#include <QObject>
#include <cudatalistener.h>
#include <cucontexti.h>
#include "qumbiaizer.h"

class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class CuContext;
class QuWatcherPrivate;

/** \brief Manage readings of various type of objects in a different thread.
  *
  * \ingroup utils
  *
  * QuWatcher is a special class that reads a tango variable using QTangoCore.
  * Any QObject or base types (int, short, double, unsigned int, QString, bool) can be
  * attached to this reader and obtain the new data when the attribute or command is
  * refreshed.
  * If a QObject is attached, then a slot must be provided together with the object so that
  * the slot is invoked with a compatible input parameter in order to update the object itself on
  * new data arrival.
  * The data type that must be managed by the QuWatcher is guessed from the slot provided or
  * detected according to one of the other attach methods available.
  *
  * \par Single shot readings
  *
  * Call setSingleShot with a true parameter to make the QuWatcher perform a only single reading.
  *
  * <strong>warning</strong>: singleShot property is read inside the refresh method. Reimplementing
  * the refresh() method in your own subclass means taking care of correctly managing the
  * singleShot property.
  *
  * @see QTangoizer::setSingleShot
  * @see QTangoizer::singleShot
  *
  * In single shot mode, newData, attributeAutoConfigured, connectionOk and
  * connectionFailed signals are still emitted.
  *
  * \par Example
  * \code
  *
  *
    QProgressBar *progressBar = new QProgressBar(this);
    QuWatcher *progressBarWatcher = new QuWatcher(this);
    progressBarWatcher->attach(progressBar, SLOT(setValue(int)));
    // configure maximum and minimum values when available
    progressBarWatcher->setAutoConfSlot(QuWatcher::Min, SLOT(setMinimum(int)));
    progressBarWatcher->setAutoConfSlot(QuWatcher::Max, SLOT(setMaximum(int)));

    progressBarWatcher->setSource("$1/short_scalar_ro");
  *
  * \endcode
  *
  * That's it! The QProgressBar will be updated via the setValue(int) slot whenever new
  * data is available and even its maximum and minumum values are updated when available
  * through the QTango auto configuration process.
  *
  * \par Example 2
  *\code
    int intVar; // want to monitor a simple integer value
    QuWatcher *intWatcher = new QuWatcher(this);
    intWatcher->attach(&intVar); // attach the integer variable to intWatcher
    intWatcher->setSource("$1/long_scalar"); // read!
  * \endcode
  *
  * See the qtango/test/watcher example inside the library distribution for an example
  *
  * \chapter Supported data types
  * \list
  * \li int
  * \li short
  * \li unsigned int
  * \li double
  * \li Tango::DevState
  * \li QString
  * \li bool
  * \li QStringList
  * \li QVector<double>
  * \li QVector<int>
  * \li QVector<bool>
  *
  * \endlist
  * \endchapter

  * <h3>Auto destruction</h3>
  * <p>
  * It is possible to setAutoDestroy to true to have your reader destroyed after the first reading.
  * <strong>Note that the object will be destroyed even if the execution fails</strong>.<br/>
  * A convenience class, introduced in cumbia 1.2.4 is Qu1TWatcher, which is a single shot, auto destroying
  * QuWatcher
  * </p>
  *
  * <h3>Single shot readings</h3>
  * <p>You can call setSingleShot to let your reader perform only one reading from the
  * source.
  *
  * @see Qu1TWatcher
  *
  * </p>
  *
  * \chapter New data signals
  * A variety of type specific signals are emitted after the value is successfully extracted from a reading.
  * A *newData* signal containing the full CuData bundle is emitted as well.
  *
  * Please note that *only data["value"]* is extracted, converted to the given type and sent through the *newData* signal.
  * Control system engines supporting *set point (or write) values* will provide *data["w_value"]* when available.
  * It is handled by QuWatcher for simple *auto configuration* tasks as aforementioned but the extracted value is not
  * propagated through *newData*.
  *
  *
  */
class QuWatcher : public Qumbiaizer, public CuDataListener, public CuContextI
{
Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource)

public:
    /** \brief Constructs a reader with parent parent.
     *
     * @see Qumbiaizer class documentation.
     */
    QuWatcher(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuWatcher(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuWatcher();

public slots:
    void setSource(const QString &s);
    void unsetSource();
    virtual bool ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool);

private:
    QuWatcherPrivate *d;


    // CuContextI interface
public:
    CuContext *getContext() const;

    // CuDataListener interface
public:
    void onUpdate(const CuData &data);
    QString source() const;

};

class Qu1TWatcher : public QuWatcher {
    Q_OBJECT
public:
    Qu1TWatcher(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);
    Qu1TWatcher(QObject *parent, Cumbia *cumbia, const CuControlsReaderFactoryI &r_factory);
    ~Qu1TWatcher();
};


#endif // READER_H
