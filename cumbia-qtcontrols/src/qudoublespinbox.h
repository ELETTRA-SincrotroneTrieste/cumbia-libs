#ifndef QuDoubleSpinBox_H
#define QuDoubleSpinBox_H

#include <QDoubleSpinBox>
#include <cudatalistener.h>
#include <cucontexti.h>

class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuControlsWriterFactoryI;

class QuDoubleSpinBoxPrivate;

class QuDoubleSpinBox : public QDoubleSpinBox, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target WRITE setTarget DESIGNABLE true)

public:
    QuDoubleSpinBox(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac);
    QuDoubleSpinBox(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);
    QuDoubleSpinBox(QWidget *parent);
    virtual ~QuDoubleSpinBox();

    // CuContextI interface
    virtual CuContext *getContext() const;

    // CuDataListener interface
    virtual void onUpdate(const CuData &data);

    QString target() const;

public slots:
    void setTarget(const QString &targets, CuContext *ctx = nullptr);
    void clearTarget();
    virtual bool ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool);

private:
    QuDoubleSpinBoxPrivate *d;

    void m_init();
};

#endif // QuDoubleSpinBox_H
