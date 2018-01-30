#ifndef QUAPPLYNUMERIC_H
#define QUAPPLYNUMERIC_H

#include <eapplynumeric.h>
#include <cudatalistener.h>
#include <cucontexti.h>

class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuControlsWriterFactoryI;

class QuApplyNumericPrivate;

class QuApplyNumeric : public EApplyNumeric, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target WRITE setTarget DESIGNABLE true)

public:
    QuApplyNumeric(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac);

    QuApplyNumeric(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuApplyNumeric();

    QString target() const;

public slots:
    virtual void execute();

    void setTarget(const QString& target);

    void execute(double val);

private:
    QuApplyNumericPrivate *d;

    void m_init();

    // CuTangoListener interface
public:
    void onUpdate(const CuData &d);

    // CuContextI interface
public:
    CuContext *getContext() const;
};


#endif // QUAPPLYNUMERIC_H
