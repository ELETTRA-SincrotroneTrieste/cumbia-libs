#ifndef CULINKSTATSCONNECTOR_H
#define CULINKSTATSCONNECTOR_H

#include <QObject>

class CuLinkStatsConnector : public QObject
{
    Q_OBJECT
public:
    explicit CuLinkStatsConnector(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CULINKSTATSCONNECTOR_H