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

/** \brief A widget to input a number and write it to a target linked to a control system
 *         engine.
 *
 *
 * \ingroup inputw
 */
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

    void setTarget(const QString& target);

    virtual void execute(double val);

    void onAnimationValueChanged(const QVariant &v);

protected:
    void paintEvent(QPaintEvent *pe);

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
