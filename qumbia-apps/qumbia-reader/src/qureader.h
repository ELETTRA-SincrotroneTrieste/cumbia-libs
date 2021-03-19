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
    void newFloat(const QString& src, double timestamp_us, float val, const CuData& da);
    void newBool(const QString& src, double timestamp_us, bool val, const CuData& da);
    void newLong(const QString& src, double timestamp_us, long val, const CuData& da);
    void newShort(const QString& src, double timestamp_us, short val, const CuData& da);
    void newUShort(const QString& src, double timestamp_us, unsigned short val, const CuData& da);
    void newULong(const QString& src, double timestamp_us, unsigned long val, const CuData& da);
    void newString(const QString& src, double timestamp_us, const QString& val, const CuData& da);
    void toString(const QString& src, const QString& fromType, double timestamp_us, const QString& val, const CuData& da);

    void newDoubleVector(const QString& src, double timestamp_us, const QVector<double>& v, const CuData& da);
    void newFloatVector(const QString& src, double timestamp_us, const QVector<float>& v, const CuData& da);
    void newBoolVector(const QString& src, double timestamp_us, const QVector<bool>& v, const CuData& da);
    void newShortVector(const QString& src, double timestamp_us, const QVector<short>& v, const CuData& da);
    void newLongVector(const QString& src, double timestamp_us, const QVector<long>& v, const CuData& da);
    void newULongVector(const QString& src, double timestamp_us, const QVector<unsigned long>& v, const CuData& da);
    void newUShortVector(const QString& src, double timestamp_us, const QVector<unsigned short>& v, const CuData& da);
    void newStringList(const QString& src, double timestamp_us, const QStringList& v, const CuData& da);
    void toStringList(const QString& src, const QString& fromType, double timestamp_us, const QStringList& v, const CuData& da);

    void newDoubleMatrix(const QString& src, double timestamp_us, const CuMatrix<double>& m, const CuData& da);
    void newFloatMatrix(const QString& src, double timestamp_us, const CuMatrix<float>& m, const CuData& da);
    void newBoolMatrix(const QString& src, double timestamp_us, const CuMatrix<bool>& m, const CuData& da);
    void newUCharMatrix(const QString& src, double timestamp_us, const CuMatrix<unsigned char>& m, const CuData& da);
    void newCharMatrix(const QString& src, double timestamp_us, const CuMatrix<char>& m, const CuData& da);
    void newShortMatrix(const QString& src, double timestamp_us, const CuMatrix<short>& m, const CuData& da);
    void newUShortMatrix(const QString& src, double timestamp_us, const CuMatrix<unsigned short>& m, const CuData& da);
    void newStringMatrix(const QString& src, double timestamp_us, const CuMatrix<std::string>& m, const CuData& da);
    void newIntMatrix(const QString& src, double timestamp_us, const CuMatrix<int>& m, const CuData& da);
    void newUIntMatrix(const QString& src, double timestamp_us, const CuMatrix<unsigned>& m, const CuData& da);
    void newULongMatrix(const QString& src, double timestamp_us, const CuMatrix<unsigned long>& m, const CuData& da);
    void newLongMatrix(const QString& src, double timestamp_us, const CuMatrix<long>& m, const CuData& da);



    void newError(const QString& src, double timestamp_us, const QString& msg, const CuData& da);

    void propertyReady(const QString& src, double timestamp_us, const CuData& p);
    void newHdbData(const QString& src, const CuData& da);


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
