#include "cucontextactionbridge.h"
#include "cucontexti.h"

#ifdef CUMBIAQTCONTROLS_HAS_QWT
#include "cuinfodialog.h"
#endif

#include <QWidget>
#include <QPoint>
#include <cumacros.h>
#include <QMenu>
#include <QAction>
#include <cumbia.h>
#include <cumbiapool.h>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <QtDebug>

class CuLinkStatsConnectorPrivate
{
public:
    CuLinkStatsConnectorPrivate() {
        cumbia = NULL;
        cupool = NULL;
        rfac = NULL;
    }

    Cumbia* cumbia;
    CumbiaPool *cupool;
    CuControlsFactoryPool fpool;
    CuControlsReaderFactoryI* rfac;
};

/** \brief the class constructor accepting a parent widget whose children implementing the linkStatsRequest signal
 *         are connected to a dialog window to display context and link statistics, a pointer to a Cumbia reference
 *         and a CuControlsReaderFactoryI const reference for the
 *         live readings.
 *
 * @param a receiver (typically a widget displaying statistics on the link) implementing the
 *        onCustomContextMenuRequested slot.
 * @param parent a QWidget whose children implementing the linkStatsRequest signal are used to provide link and context
 *        statistics.
 *
 * @see CuContext
 *
 *
 */
CuContextActionBridge::CuContextActionBridge(QWidget *parent, Cumbia* cumbia, const CuControlsReaderFactoryI &r_fac)
    : QObject(parent)
{
    d = new CuLinkStatsConnectorPrivate;
    d->cumbia = cumbia;
    d->rfac = r_fac.clone();

    m_connect();
}

/** \brief the class constructor accepting a parent widget whose children implementing the linkStatsRequest signal
 *         are connected to a dialog window to display context and link statistics, a pointer to a CumbiaPool instance
 *         and a CuControlsReaderFactoryI const reference for the
 *         live readings.
 *
 * @param a receiver (typically a widget displaying statistics on the link) implementing the
 *        onCustomContextMenuRequested slot.
 * @param parent a QWidget whose children implementing the linkStatsRequest signal are used to provide link and context
 *        statistics.
 *
 * @see CuContext
 *
 *
 */
CuContextActionBridge::CuContextActionBridge(QWidget *parent, CumbiaPool *cumbia_pool,
                                             const CuControlsFactoryPool &fpool)
    : QObject(parent)
{
    d = new CuLinkStatsConnectorPrivate;
    d->cupool = cumbia_pool;
    d->fpool = fpool;
    m_connect();
}

CuContextActionBridge::~CuContextActionBridge()
{
    if(d->rfac)
        delete d->rfac;
    delete d;
}

/** \brief Register an object implementing the linkStatsRequest(QWidget*, CuContextI *) signal.
 *
 * This method can be used to connect an object o if it was not available as a child of the main
 * widget when the CuContextActionBridge had been instantiated.
 *
 * @param o the object to register.
 */
void CuContextActionBridge::connectObject(QObject *o)
{
    bool res = QObject::connect(o, SIGNAL(linkStatsRequest(QWidget*, CuContextI *)),
                                this, SIGNAL(linkStatsRequest(QWidget*, CuContextI *)));

    if(!res)
        perr("CuLinkStatsConnector.connectObject: object \"%s\" of type %s must implement the\n"
             "linkStatsRequest(QWidget*) SIGNAL",
             qstoc(o->objectName()), o->metaObject()->className());
}

/// @private // no doxygen
void CuContextActionBridge::onLinkStatsRequest(QWidget *sender, CuContextI *w)
{
#ifdef CUMBIAQTCONTROLS_HAS_QWT

    if(d->cumbia)
    {
        // WA_DeleteOnClose attribute is set
        CuInfoDialog* dlg = new CuInfoDialog(0, d->cumbia, d->rfac);
        dlg->exec(sender, w);
    }
    else if(d->cupool)
    {
        // WA_DeleteOnClose attribute is set
        CuInfoDialog* dlg = new CuInfoDialog(0, d->cupool, d->fpool);
        dlg->exec(sender, w);
    }
#else
#endif
}

/// @private // no doxygen
void CuContextActionBridge::m_connect()
{
    foreach(QWidget *w, qobject_cast<QWidget *>(parent())->findChildren<QWidget *>())
    {
        const QMetaObject *mo = w->metaObject();
        bool hasSignal = mo->indexOfSignal(mo->normalizedSignature("linkStatsRequest(QWidget*, CuContextI *)")) > -1;
  //      qDebug() << __FUNCTION__ << mo->className() << " has linkStatsRequest signal: " << hasSignal;
        if(hasSignal)
        {
            connect(w, SIGNAL(linkStatsRequest(QWidget*, CuContextI *)),
                          this, SLOT(onLinkStatsRequest(QWidget*, CuContextI *)));
        }
    }
}

