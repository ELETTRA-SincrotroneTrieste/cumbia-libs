#ifndef CUFORMULASREADER_H
#define CUFORMULASREADER_H

#include <QString>
#include <cucontrolsreader_abs.h>
#include <cucontrolsfactories_i.h>
#include <cudata.h>
#include <cudataquality.h>
#include <QScriptValue>
#include <QEvent>
#include <QThread>

class CuFormulaReaderFactoryPrivate;
class CumbiaPool;
class CuControlsFactoryPool;

class SetSrcFailedEvent : public QEvent {
public:
    enum EventType { SetSrcFailedType = QEvent::User + 205 } ;

    SetSrcFailedEvent(const QString &src, const QString& msg);

    ~SetSrcFailedEvent();

    QString source, message;
};

class CuFormulaReaderFactory : public CuControlsReaderFactoryI
{
    // CuControlsWriterFactoryI interface
public:
    CuFormulaReaderFactory(CumbiaPool *cu_poo, const CuControlsFactoryPool &fpool);

    virtual ~CuFormulaReaderFactory();

    CuControlsReaderA *create(Cumbia *c, CuDataListener *l) const;

    void setOptions(const CuData& o);

    CuData getOptions() const;

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

private:
    CuFormulaReaderFactoryPrivate *d;
};

class CuFormulaReaderPrivate;

class CuFormulaReader : public QObject, public CuControlsReaderA
{
    Q_OBJECT
public:
    CuFormulaReader(Cumbia *c, CuDataListener *l, CumbiaPool *cu_poo, const CuControlsFactoryPool &fpool);

    enum RefreshMode { RefreshModeUndefined = 0x1,  Event = 0x2, Polled = 0x4, OneShot = 0x8 };

    ~CuFormulaReader();

    bool error() const;

    QString message() const;

    CuDataQuality combinedQuality() const;

    std::string combinedMessage() const;

    std::string combinedSources() const;

    std::string combinedModes() const;

    std::vector<bool> errors() const;

    CuVariant fromScriptValue(const QScriptValue &v);

    // CuControlsReaderA interface
public:
    void setSource(const QString &s);
    QString source() const;
    void unsetSource();
    void sendData(const CuData &d);
    void getData(CuData &d_ino) const;
    void setOptions(const CuData& opt);

public slots:
    void onNewData(const CuData & da);

protected:
    bool event(QEvent *e);

private:
    CuFormulaReaderPrivate *d;

    bool m_allValuesValid() const;

    void m_disposeWatchers();

    QScriptValue m_getScalarVal(const CuVariant& v);
    QScriptValue m_getVectorVal(const CuVariant& v);

    CuVariant::DataType getScriptValueType(const QScriptValue& v) const;

    void m_notifyFormulaError();

    void m_srcReplaceWildcards();

    QString m_makeSetSrcError();

    bool m_checkDisplayUnit(const CuVariant& duv);

    RefreshMode m_getRefreshMode(const std::string& mode) const;
};

#endif // CUFORMULASREADER_H
