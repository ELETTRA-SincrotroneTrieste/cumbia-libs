#include "cucontextactionbridge.h"
#include "cucontextwidgeti.h"
#include "cuinfodialog.h"
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

/** \brief Connect the receiver onCustomContextMenuRequested slot to the customContextMenuRequested
 *         signal of the widgets children of parent whose contextMenuPolicy is Qt::CustomContextMenu
 *         and so emit the customContextMenuRequested signal when  the user has requested a context
 *         menu on the widget.
 *
 * @param a receiver (typically a widget displaying statistics on the link) implementing the
 *        onCustomContextMenuRequested slot.
 * @param parent a QWidget where children with the contextMenuPolicy set to Qt::CustomContextMenu
 *        are searched for and connected to the aforementioned receiver's slot.
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

/** \brief Explicitly connect an object implementing the onCustomContextMenuRequested(QPoint)
 *         slot to another object o, implementing the customContextMenuRequested(QPoint) signal.
 *
 * This method can be used to connect the object o if it is not a child
 * of the parent object that had been passed to the CuLinkStatsConnector constructor.
 *
 * @see CuLinkStatsConnector::CuLinkStatsConnector
 */
void CuContextActionBridge::connectObject(QObject *o)
{
    bool res = QObject::connect(o, SIGNAL(linkStatsRequest(QWidget*, CuContextWidgetI *)),
                                this, SIGNAL(linkStatsRequest(QWidget*, CuContextWidgetI *)));

    if(!res)
        perr("CuLinkStatsConnector.connectObject: object \"%s\" of type %s must implement the\n"
             "linkStatsRequest(QWidget*) SIGNAL",
             qstoc(o->objectName()), o->metaObject()->className());
}

void CuContextActionBridge::onLinkStatsRequest(QWidget *sender, CuContextWidgetI *w)
{
    if(d->cumbia)
    {
        CuInfoDialog dlg(0, d->cumbia, d->rfac);
        dlg.exec(sender, w);
    }
    else if(d->cupool)
    {
        CuInfoDialog dlg(0, d->cupool, d->fpool);
        dlg.exec(sender, w);
    }
}

void CuContextActionBridge::m_connect()
{
    bool res;
    foreach(QWidget *w, qobject_cast<QWidget *>(parent())->findChildren<QWidget *>())
    {
        const QMetaObject *mo = w->metaObject();
        bool hasSignal = mo->indexOfSignal(mo->normalizedSignature("linkStatsRequest(QWidget*, CuContextWidgetI *)")) > -1;
        qDebug() << __FUNCTION__ << mo->className() << " has linkStatsRequest signal: " << hasSignal;
        if(hasSignal)
        {
            res = connect(w, SIGNAL(linkStatsRequest(QWidget*, CuContextWidgetI *)),
                          this, SLOT(onLinkStatsRequest(QWidget*, CuContextWidgetI *)));
            if(!res)
                perr("CuLinkStatsConnector.connectObject: error connecting linkStatsRequest signal of object %s [%s]",
                     qstoc(w->objectName()), w->metaObject()->className());
            else
                printf("connected %s [%s]\n",
                       qstoc(w->objectName()), w->metaObject()->className());
        }
    }
}

