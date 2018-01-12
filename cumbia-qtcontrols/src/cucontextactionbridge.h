#ifndef CUCONTEXTACTIONBRIDGE_H
#define CUCONTEXTACTIONBRIDGE_H

#include <QObject>

class CuLinkStatsConnectorPrivate;
class QPoint;
class CuContextI;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;

/** \brief All the children of a parent widget that implement the signal \a linkStatsRequest can be bridged
 *         by this class to a CuInfoDialog dialog window that shows statistics about the context of a cumbia-qtcontrols
 *         widget.
 *
 * @see CuContext
 * @see CuContextManu
 *
 */
class CuContextActionBridge : public QObject
{
    Q_OBJECT
public:
    explicit CuContextActionBridge(QWidget *parent, Cumbia* cumbia, const CuControlsReaderFactoryI &r_fac);

    explicit CuContextActionBridge(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~CuContextActionBridge();

    void connectObject(QObject *w);

private slots:
    void onLinkStatsRequest(QWidget *sender, CuContextI *);

private:
    CuLinkStatsConnectorPrivate *d;

    void m_connect();
};

#endif // CULINKSTATSCONNECTOR_H
