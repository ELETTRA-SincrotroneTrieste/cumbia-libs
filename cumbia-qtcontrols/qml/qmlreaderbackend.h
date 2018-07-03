#ifndef CIRCULARGAUGEBACKEND_H
#define CIRCULARGAUGEBACKEND_H

#include <QObject>
#include <QVariant>
#include "cudatalistener.h"
#include "cucontexti.h"

class Cumbia;
class CumbiaPool;
class CuVariant;
class CuControlsFactoryPool;
class CuControlsReaderFactoryI;
class QmlReaderBackendPrivate;
class CumbiaPool_O;

class QmlReaderBackend : public  QObject, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value NOTIFY valueChanged)
    Q_PROPERTY(QVariant min READ min NOTIFY minChanged)
    Q_PROPERTY(QVariant max READ max NOTIFY maxChanged)
    Q_PROPERTY(QVariant min_warning READ min_warning NOTIFY min_warningChanged)
    Q_PROPERTY(QVariant max_warning READ max_warning NOTIFY max_warningChanged)
    Q_PROPERTY(QVariant min_alarm READ min_alarm NOTIFY min_alarmChanged)
    Q_PROPERTY(QVariant max_alarm READ max_alarm NOTIFY max_alarmChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString label READ label NOTIFY labelChanged)
    Q_PROPERTY(QString stateColor READ stateColor NOTIFY stateColorChanged)
    Q_PROPERTY(QString qualityColor READ qualityColor NOTIFY qualityColorChanged)

public:

    enum DataType { Int, Double, String, Boolean };

    enum DataFormat { Scalar, Vector };

    QmlReaderBackend(QObject *parent = nullptr);

    Q_INVOKABLE void init(CumbiaPool_O *poo_o);

    Q_INVOKABLE void setConversionMode(DataType dt, DataFormat fmt);

    Q_INVOKABLE QString getQualityColor(double val);

    virtual ~QmlReaderBackend();

    QString source() const;

    CuContext *getContext() const;

    QVariant value() const;

    QVariant min() const;

    QVariant max() const;

    QVariant min_warning() const;

    QVariant max_warning() const;

    QVariant min_alarm() const;

    QVariant max_alarm() const;

    QString label() const;

    QString stateColor() const;

    QString qualityColor() const;

    // CuDataListener interface
    void onUpdate(const CuData &d);

public slots:
    void setSource(const QString& s);

    void unsetSource();

signals:
    void newData(const CuData&);

    void valueChanged(QVariant d);
    void sourceChanged(const QString& s);
    void minChanged(QVariant min);
    void maxChanged(QVariant max);
    void min_warningChanged(QVariant max);
    void max_warningChanged(QVariant max);
    void min_alarmChanged(QVariant max);
    void max_alarmChanged(QVariant max);

    void labelChanged(const QString& label);

    void stateColorChanged(const QString& color);
    void qualityColorChanged(const QString& color);

private:

    void m_init();

    void m_configure(const CuData& da);

    void m_set_value(const CuVariant &val);

    QmlReaderBackendPrivate *d;
};

#endif // CIRCULARGAUGEBACKEND_H
