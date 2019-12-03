#ifndef __QU_READER_H
#define __QU_READER_H

#include <cudatalistener.h>
#include <QObject>
#include <cudata.h>

class CumbiaPool;
class CuControlsFactoryPool;
class CuContext;

class Qu_Reader : public QObject, public CuDataListener
{
    Q_OBJECT
public:
    Qu_Reader(QObject *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    ~Qu_Reader();

    void propertyOnly();

    void saveProperty();

    void setTgPropertyList(const QStringList& props);

    void setContextOptions(const CuData& options);

    // CuDataListener interface
public:
    virtual void onUpdate(const CuData &data);

    QString source() const;

    void stop();

    void setPeriod(int ms);

signals:
    void newDouble(const QString& src, double timestamp_us, double val, const CuData& da);

    void newDoubleVector(const QString& src, double timestamp_us, const QVector<double>& v, const CuData& da);

    void newShort(const QString& src, double timestamp_us, short val, const CuData& da);

    void newShortVector(const QString& src, double timestamp_us, const QVector<short>& v, const CuData& da);

    void newLongVector(const QString& src, double timestamp_us, const QVector<long>& v, const CuData& da);

    void newLong(const QString& src, double timestamp_us, long val, const CuData& da);

    void newError(const QString& src, double timestamp_us, const QString& msg, const CuData& da);

    void propertyReady(const QString& src, double timestamp_us, const CuData& p);


public slots:
    void setSource(const QString& s);
    void getTgProps();

private:
    CuContext *m_context;
    bool m_save_property;
    QStringList m_tg_property_list;
    bool m_property_only;
    CuData m_prop;
};

#endif // READER_H
