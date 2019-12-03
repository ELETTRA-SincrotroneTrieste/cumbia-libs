#ifndef Qumbiareader_H
#define Qumbiareader_H

#include <QObject>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <QMap>

#include "rconfig.h"

class CuData;
class CumbiaPool;
class RConfig;

class QumbiaReader : public QObject
{
    Q_OBJECT

public:
    enum Verbosity { Low, Medium, High, Debug };

    explicit QumbiaReader(CumbiaPool *cu_p, QWidget *parent = 0);
    ~QumbiaReader();

    QString makeTimestamp(const double d) const;
    bool usage_only() const;

public slots:
    void onNewDouble(const QString &src, double ts, double val, const CuData& da);
    void onNewDoubleVector(const QString &src, double ts, const QVector<double> &v, const CuData& da);
    void onNewShort(const QString &src, double ts, short val, const CuData& da);
    void onNewLong(const QString &src, double ts, long val, const CuData& da);
    void onNewShortVector(const QString &src, double ts, const QVector<short> &v, const CuData& da);
    void onNewLongVector(const QString &src, double ts, const QVector<long> &v, const CuData &da);
    void onPropertyReady(const QString& src, double ts, const CuData& p);
    void onError(const QString& src, double ts, const QString& msg, const CuData& da);

private slots:
    void onReaderDestroyed(QObject *o);


private:
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    QList <QObject *> m_readers;
    RConfig m_conf;

    void m_print_extra1(const CuData& da);
    void m_print_extra2(const CuData& da);

    void m_checkRefreshCnt(QObject *o);
    void m_createReaders(const QStringList &srcs);

    void m_print_list_props(const CuData& pr);

    void m_print_property(const CuData& pr);

    template <typename T >
    QString m_format(const T& v, const char *fmt) const;

    QMap<QString, int> m_refreshCntMap;
    QMap<int, QStringList> m_props_map;

};

#endif // Cumbiareader_H
