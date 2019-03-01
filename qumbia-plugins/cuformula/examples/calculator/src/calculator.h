#ifndef Calculator_H
#define Calculator_H

#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <cumbiatango.h>
#include <cumbiaepics.h>
#include <cudatalistener.h>

class CuData;
class CumbiaPool;
class CuContext;

class Calculator : public QObject, public CuDataListener
{
    Q_OBJECT

public:
    explicit Calculator(CumbiaPool *cu_p, QObject *parent = 0, bool verbose = false);
    ~Calculator();

    void read(const QString& expr);

    bool hasFinished() const;

    int code() const;

private slots:

signals:

    void finished();

private:

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    CuContext *m_ctx;

    bool m_verbose;
    bool m_finished;
    int m_code;
    // CuDataListener interface
public:
    void onUpdate(const CuData &data);
};

#endif // Calculator_H
