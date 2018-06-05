#include "cumbiareader.h"

#include <cumbiapool.h>
#include <cumbiaepics.h>
#include <cumbiatango.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cucontextactionbridge.h>
#include <cutango-world.h>
#include <cuepics-world.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>
#include <reader.h>
#include <QCoreApplication>
#include <QDateTime>

Cumbiareader::Cumbiareader(CumbiaPool *cumbia_pool, QWidget *parent) :
    QObject(parent)
{
    cu_pool = cumbia_pool;
    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());

    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());

    // log
    cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));

    int interval = 1000;
    m_truncate = -1;

    bool ok;
    QStringList srcs;

    // non src args?
    foreach(QString a, qApp->arguments()) {
        if(a.toInt(&ok) && ok)
            interval = a.toInt();
        else if(a == "--truncate")
            m_truncate = 12;
        else if(a.startsWith("--truncate=")) {
            QString t(a);
            t.remove("--truncate=");
            if(t.toInt(&ok) && ok)
                m_truncate = t.toInt();
            else
                m_truncate = 12;
        }
        else
            srcs.append(a);
    }

    for(int i = 1; i < srcs.size(); i++)
    {
        QString a = srcs.at(i);
        if(a.toInt(&ok) && ok)
            interval = a.toInt();
        else {
            Reader *r = new Reader(this, cu_pool, m_ctrl_factory_pool);
            connect(r, SIGNAL(newDouble(QString,double,double)), this, SLOT(onNewDouble(QString,double,double)));
            connect(r, SIGNAL(newDoubleVector(QString,double,QVector<double>)), this,
                    SLOT(onNewDoubleVector(QString,double,QVector<double>)));
            connect(r, SIGNAL(newShort(QString,double,short)), this,
                    SLOT(onNewShort(QString,double,short)));
            connect(r, SIGNAL(newShortVector(QString,double,QVector<short>)), this,
                    SLOT(onNewShortVector(QString,double,QVector<short>)));

            connect(r, SIGNAL(newLong(QString,double,long)), this,
                    SLOT(onNewLong(QString,double,long)));
            connect(r, SIGNAL(newLongVector(QString,double,QVector<long>)), this,
                    SLOT(onNewLongVector(QString,double,QVector<long>)));

            connect(r, SIGNAL(newError(QString,double,QString)), this, SLOT(onError(QString,double,QString)));
            r->setSource(a);
            r->setPeriod(interval);
        }
    }

}

Cumbiareader::~Cumbiareader()
{

}

QString Cumbiareader::makeTimestamp(const double d) const {
    QString ts = QDateTime::fromSecsSinceEpoch(static_cast<int>(d)).toString("HH:mm:ss");
    ts += QString("+%1").arg(d - static_cast<int>(d));
    return ts;
}

void Cumbiareader::onNewDouble(const QString& src, double ts, double val)
{
    printf("%s: [%s]  [\e[1;36mdouble\e[0m] \e[1;32m%f\e[0m\n", qstoc(src), qstoc(makeTimestamp(ts)), val);
}

void Cumbiareader::onNewDoubleVector(const QString &src, double ts, const QVector<double> &v)
{
    printf("%s: [%s] [\e[1;36mdouble\e[0m,%d] { \e[0;32m", qstoc(src), qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_truncate < 0 || i < m_truncate - 1); i++)
        printf("%g,", v[i]);
    if(m_truncate > -1 && m_truncate < v.size())
        printf(" ..., ");
    printf("%.2f \e[0m}\n", v[v.size() - 1]);
}

void Cumbiareader::onNewShort(const QString &src, double ts, short val)
{
    printf("%s: [%s] [\e[1;34mshort\e[0m] \e[1;32m%d\e[0m\n", qstoc(src), qstoc(makeTimestamp(ts)), val);
}

void Cumbiareader::onNewLong(const QString &src, double ts, long val)
{
    printf("%s: [%s] [\e[1;34mshort\e[0m] \e[1;32m%ld\e[0m\n", qstoc(src), qstoc(makeTimestamp(ts)), val);
}

void Cumbiareader::onNewLongVector(const QString &src, double ts, const QVector<long> &v)
{
    printf("%s: [%s] [\e[1;35mlong\e[0m,%d] { \e[0;32m", qstoc(src), qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_truncate < 0 || i < m_truncate - 1); i++)
        printf("%ld,", v[i]);
    if(m_truncate > -1 && m_truncate < v.size())
        printf(" ..., ");
    printf("%ld \e[0m}\n", v[v.size() - 1]);
}

void Cumbiareader::onNewShortVector(const QString &src, double ts, const QVector<short> &v)
{
    printf("%s: [%s] [\e[1;34mshort\e[0m,%d] { \e[0;32m", qstoc(src), qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_truncate < 0 || i < m_truncate - 1); i++)
        printf("%d,", v[i]);
    if(m_truncate > -1 && m_truncate < v.size())
        printf(" ..., ");
    printf("%d \e[0m}\n", v[v.size() - 1]);
}

void Cumbiareader::onError(const QString &src, double ts, const QString &msg)
{
    perr("%s: [%s]: \"\e[1;31m%s\e[0m\"\n", qstoc(src), qstoc(makeTimestamp(ts)), qstoc(msg));
}
