#ifndef QULED_H
#define QULED_H

#include <eled.h>
#include <cudatalistener.h>

class QuLedPrivate;
class Cumbia;
class CuControlsReaderFactoryI;
class CumbiaPool;
class CuControlsFactoryPool;
class CuContext;

class QuLed : public ELed, public CuDataListener
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:
    QuLed(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuLed(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuLed();

    QString source() const;

    CuContext *getContext() const;

public slots:
    void setSource(const QString& s);

    void unsetSource();

signals:
    void newData(const CuData&);

protected:

private:
    QuLedPrivate *d;

    void m_init();

    // CuTangoListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QULED_H
