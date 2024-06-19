#ifndef QUSPINBOX_H
#define QUSPINBOX_H

#include <QSpinBox>
#include <cudatalistener.h>
#include <cucontexti.h>

class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuControlsWriterFactoryI;

class QuSpinBoxPrivate;

class QuSpinBox : public QSpinBox, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target WRITE setTarget DESIGNABLE true)

public:
    QuSpinBox(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac);
    QuSpinBox(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);
    QuSpinBox(QWidget *parent);
    virtual ~QuSpinBox();

    // CuContextI interface
    virtual CuContext *getContext() const;

    // CuDataListener interface
    virtual void onUpdate(const CuData &data);

    QString target() const;

public slots:
    void setTarget(const QString &targets, CuContext *ctx = nullptr);
    void clearTarget();
    bool ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool);
private:
    QuSpinBoxPrivate *d;

    void m_init();
};

#endif // QUSPINBOX_H
