#include "cucontextactionbridge.h"
#include <QWidget>
#include <QPoint>
#include <cumacros.h>
#include <QMenu>
#include <QAction>

class CuLinkStatsConnectorPrivate
{
public:
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
CuContextActionBridge::CuContextActionBridge(QWidget *parent) : QObject(parent)
{
    bool res;
    d = new CuLinkStatsConnectorPrivate;

    foreach(QWidget *w, parent->findChildren<QWidget *>())
    {
        const QMetaObject *mo = w->metaObject();
        if(mo->indexOfSignal(mo->normalizedSignature("linkStatsRequest(QWidget*)")) > -1)
        {
            res = connect(w, SIGNAL(linkStatsRequest(QWidget*)),
                          this, SIGNAL(linkStatsRequest(QWidget*)));
            if(!res)
                perr("CuLinkStatsConnector.connectObject: error connecting linkStatsRequest signal of object %s [%s]",
                     qstoc(w->objectName()), w->metaObject()->className());
            else
                printf("connected %s [%s]\n",
                       qstoc(w->objectName()), w->metaObject()->className());
        }
    }
}

CuContextActionBridge::~CuContextActionBridge()
{
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
    bool res = QObject::connect(o, SIGNAL(linkStatsRequest(QWidget*)),
                                this, SIGNAL(linkStatsRequest(QWidget*)));

    if(!res)
        perr("CuLinkStatsConnector.connectObject: object \"%s\" of type %s must implement the\n"
             "linkStatsRequest(QWidget*) SIGNAL",
             qstoc(o->objectName()), o->metaObject()->className());
}

