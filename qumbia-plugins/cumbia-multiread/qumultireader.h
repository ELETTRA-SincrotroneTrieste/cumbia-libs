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

    void init(Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);
    void init(CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);
    void setSources(const QStringList &srcs, bool sequential  = false);
    void unsetSources();
    void addSource(const QString &src, int after);
    void removeSource(const QString &src);
    QStringList sources() const;

    int period() const;

    void setPeriod(int ms);

signals:
    void onNewData(const CuData& da);
    void onSeqReadComplete(const QList<CuData >& data);

private:
    QuMultiReaderPrivate *d;

    // CuDataListener interface
public:
    void onUpdate(const CuData &data);
};

#endif // QUMULTIREADER_H
