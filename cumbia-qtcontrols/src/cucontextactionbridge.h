#ifndef CUCONTEXTACTIONBRIDGE_H
#define CUCONTEXTACTIONBRIDGE_H

#include <QObject>

class CuLinkStatsConnectorPrivate;
class QPoint;

class CuContextActionBridge : public QObject
{
    Q_OBJECT
public:
    explicit CuContextActionBridge(QWidget *parent);

    virtual ~CuContextActionBridge();

    void connectObject(QObject *w);

signals:
    void linkStatsRequest(QWidget *sender);

private:
    CuLinkStatsConnectorPrivate *d;
};

#endif // CULINKSTATSCONNECTOR_H
