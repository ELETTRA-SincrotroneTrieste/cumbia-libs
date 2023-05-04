#ifndef QULINEEDIT_H
#define QULINEEDIT_H

#include <QLineEdit>
#include <cudatalistener.h>
#include <cucontexti.h>

class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuControlsWriterFactoryI;

class QuLineEditPrivate;

class QuLineEdit : public QLineEdit, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target WRITE setTarget DESIGNABLE true)

public:
    QuLineEdit(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac);

    QuLineEdit(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuLineEdit();

    // CuContextI interface
    virtual CuContext *getContext() const;

    // CuDataListener interface
    virtual void onUpdate(const CuData &data);

    QString target() const;

public slots:
    void setTarget(const QString &targets);
    virtual bool ctxSwap(CumbiaPool *cu_p, const CuControlsFactoryPool &fpool);

private:
    QuLineEditPrivate *d;

    void m_init();
};

#endif // QuLineEdit_H
