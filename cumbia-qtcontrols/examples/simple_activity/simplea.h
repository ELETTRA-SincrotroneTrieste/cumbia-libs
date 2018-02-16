#ifndef SIMPLEA_H
#define SIMPLEA_H

#include <QWidget>
#include <cuthreadlistener.h>
#include <cuthreadseventbridge.h>

class Cumbia;

class QThreadsEventBridgeFactory : public CuThreadsEventBridgeFactory_I
{
public:

    // CuThreadsEventBridgeFactory_I interface
public:
    CuThreadsEventBridge_I *createEventBridge() const;
};

class SimpleA : public QWidget, public CuThreadListener
{
    Q_OBJECT
public:
    explicit SimpleA(QWidget *parent = nullptr);

private slots:
    void start();

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    CuData getToken() const;

private:
    Cumbia *m_cumbia;
};

#endif // SIMPLEA_H
