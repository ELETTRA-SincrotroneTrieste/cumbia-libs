#ifndef READER_H
#define READER_H

#include <com_proxy_reader.h>
#include <QObject>


class Reader : public QObject, public QTangoComProxyReader
{
    Q_OBJECT
public:
    Reader(QObject *parent);

    ~Reader();

protected slots:

    virtual void refresh(const TVariant &);

signals:
    void newDouble(const QString& src, double timestamp_us, double val);

    void newDoubleVector(const QString& src, double timestamp_us, const QVector<double>& v);

    void newInt(const QString& src, double timestamp_us, int val);

    void newIntVector(const QString& src, double timestamp_us, const QVector<int>& v);

    void newError(const QString& src, double timestamp_us, const QString& msg);

private:

};

#endif // READER_H
