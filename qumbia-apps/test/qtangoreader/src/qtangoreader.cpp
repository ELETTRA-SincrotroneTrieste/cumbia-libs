#include "qtangoreader.h"


#include <macros.h>
#include <reader.h>
#include <QCoreApplication>
#include <QDateTime>

QTangoreader::QTangoreader(QObject *parent) :
    QObject(parent)
{
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
            Reader *r = new Reader(this);
            connect(r, SIGNAL(newDouble(QString,double,double)), this, SLOT(onNewDouble(QString,double,double)));
            connect(r, SIGNAL(newDoubleVector(QString,double,QVector<double>)), this,
                    SLOT(onNewDoubleVector(QString,double,QVector<double>)));
            connect(r, SIGNAL(newInt(QString,double,int)), this,
                    SLOT(onNewInt(QString,double,int)));
            connect(r, SIGNAL(newIntVector(QString,double,QVector<int>)), this,
                    SLOT(onNewIntVector(QString,double,QVector<int>)));
            connect(r, SIGNAL(newError(QString,double,QString)), this, SLOT(onError(QString,double,QString)));
            r->setPeriod(interval);
            r->setSource(a);
        }
    }

}

QTangoreader::~QTangoreader()
{

}

QString QTangoreader::makeTimestamp(const double d) const {
    QString ts = QDateTime::fromSecsSinceEpoch(static_cast<int>(d)).toString("HH:mm:ss");
    ts += QString("+%1").arg(d - static_cast<int>(d));
    return ts;
}

void QTangoreader::onNewDouble(const QString& src, double ts, double val)
{
    printf("%s: [%s]  [\e[1;36mdouble\e[0m] \e[1;32m%f\e[0m\n", qstoc(src), qstoc(makeTimestamp(ts)), val);
}

void QTangoreader::onNewDoubleVector(const QString &src, double ts, const QVector<double> &v)
{
    printf("%s: [%s] [\e[1;36mdouble\e[0m,%d] { \e[0;32m", qstoc(src), qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_truncate < 0 || i < m_truncate - 1); i++)
        printf("%g,", v[i]);
    if(m_truncate > -1 && m_truncate < v.size())
        printf(" ..., ");
    printf("%.2f \e[0m}\n", v[v.size() - 1]);
}

void QTangoreader::onNewInt(const QString &src, double ts, int val)
{
    printf("%s: [%s] [\e[1;34mint\e[0m] \e[1;32m%d\e[0m\n", qstoc(src), qstoc(makeTimestamp(ts)), val);
}

void QTangoreader::onNewIntVector(const QString &src, double ts, const QVector<int> &v)
{
    printf("%s: [%s] [\e[1;34mint\e[0m,%d] { \e[0;32m", qstoc(src), qstoc(makeTimestamp(ts)), v.size());
    for(int i = 0; i < v.size() -1 && (m_truncate < 0 || i < m_truncate - 1); i++)
        printf("%d,", v[i]);
    if(m_truncate > -1 && m_truncate < v.size())
        printf(" ..., ");
    printf("%d \e[0m}\n", v[v.size() - 1]);
}

void QTangoreader::onError(const QString &src, double ts, const QString &msg)
{
    perr("%s: [%s]: \"\e[1;31m%s\e[0m\"\n", qstoc(src), qstoc(makeTimestamp(ts)), qstoc(msg));
}
