#ifndef CUWSCLIENT_H
#define CUWSCLIENT_H

#include <QObject>

class CuWSClient : public QObject
{
    Q_OBJECT
public:
    explicit CuWSClient(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CUWSCLIENT_H