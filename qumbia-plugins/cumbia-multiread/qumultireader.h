#ifndef QUMULTIREADER_H
#define QUMULTIREADER_H

#include <QObject>
#include <QList>
#include <qumultireaderplugininterface.h>
#include <cudata.h>
#include <cudatalistener.h>

class QuMultiReaderPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;

class QuMultiReader : public QObject, public QuMultiReaderPluginInterface, public CuDataListener
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "cumbia-multiread.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(QuMultiReaderPluginInterface)

public:
    QuMultiReader(QObject *parent = 0);

    virtual ~QuMultiReader();

    // QuMultiReaderPluginInterface interface
public:

    void init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac, int manual_mode_code = -1);
    void init(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool, int manual_mode_code = -1);
    void setSources(const QStringList &srcs);
    void unsetSources();
    void insertSource(const QString &src, int i);
    void removeSource(const QString &src);
    const QObject *get_qobject() const;
    QStringList sources() const;

    int period() const;

    void setPeriod(int ms);

public slots:
    void startRead();

signals:
    void onNewData(const CuData& da);
    void onSeqReadComplete(const QList<CuData >& data);

private:
    QuMultiReaderPrivate *d;

    void m_startTimer();

    // CuDataListener interface
public:
    void onUpdate(const CuData &data);
};

#endif // QUMULTIREADER_H
