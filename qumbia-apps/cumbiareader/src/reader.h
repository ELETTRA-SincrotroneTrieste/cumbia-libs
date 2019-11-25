#ifndef READER_H
#define READER_H

#include <cudatalistener.h>
#include <QObject>

class CumbiaPool;
class CuControlsFactoryPool;
class CuContext;

class Reader : public QObject, public CuDataListener
{
    Q_OBJECT
public:
    Reader(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    ~Reader();

    // CuDataListener interface
public:
    virtual void onUpdate(const CuData &data);

    QString source() const;

    void setPeriod(int ms);

signals:
    void newDouble(const QString& src, double timestamp_us, double val);

    void newDoubleVector(const QString& src, double timestamp_us, const QVector<double>& v);

    void newShort(const QString& src, double timestamp_us, short val);

    void newShortVector(const QString& src, double timestamp_us, const QVector<short>& v);

    void newLongVector(const QString& src, double timestamp_us, const QVector<long>& v);

    void newLong(const QString& src, double timestamp_us, long val);

    void newError(const QString& src, double timestamp_us, const QString& msg);



public slots:
    void setSource(const QString& s);

private:
    CuContext *m_context;
};

#endif // READER_H
