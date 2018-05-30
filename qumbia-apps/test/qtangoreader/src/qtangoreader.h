#ifndef QTangoreader_H
#define QTangoreader_H

#include <QObject>

class QTangoreader : public QObject
{
    Q_OBJECT

public:
    explicit QTangoreader(QObject *parent = 0);
    ~QTangoreader();

    QString makeTimestamp(const double d) const;
public slots:
    void onNewDouble(const QString &src, double ts, double val);
    void onNewDoubleVector(const QString &src, double ts, const QVector<double> &v);
    void onNewInt(const QString &src, double ts, int val);
    void onNewIntVector(const QString &src, double ts, const QVector<int> &v);

    void onError(const QString& src, double ts, const QString& msg);


private:

    int m_truncate;
};

#endif // QTangoreader_H
