#ifndef Cumbiareader_H
#define Cumbiareader_H

#include <QObject>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <cumbiatango.h>
#include <cumbiaepics.h>

class CuData;
class CumbiaPool;

class Cumbiareader : public QObject
{
    Q_OBJECT

public:
    explicit Cumbiareader(CumbiaPool *cu_p, QWidget *parent = 0);
    ~Cumbiareader();

    QString makeTimestamp(const double d) const;
public slots:
    void onNewDouble(const QString &src, double ts, double val);
    void onNewDoubleVector(const QString &src, double ts, const QVector<double> &v);
    void onNewShort(const QString &src, double ts, short val);
    void onNewLong(const QString &src, double ts, long val);
    void onNewShortVector(const QString &src, double ts, const QVector<short> &v);
    void onNewLongVector(const QString &src, double ts, const QVector<long> &v);

    void onError(const QString& src, double ts, const QString& msg);


private:
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;

    int m_truncate;
};

#endif // Cumbiareader_H
