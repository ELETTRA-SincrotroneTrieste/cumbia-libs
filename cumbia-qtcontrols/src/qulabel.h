#ifndef QULABEL_H
#define QULABEL_H

#include <esimplelabel.h>
#include <cudatalistener.h>

class QuLabelPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;

class QuLabel : public ESimpleLabel, public CuDataListener
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:
    QuLabel(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuLabel(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuLabel();

    QString source() const;

    int maximumLength() const;


public slots:
    void setSource(const QString& s);

    void unsetSource();

    void setMaximumLength(int len);

signals:
    void newData(const CuData&);

protected:

private:
    QuLabelPrivate *d;

    void m_init();

    // CuTangoListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUTLABEL_H
