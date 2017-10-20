#ifndef QUTABLE_H
#define QUTABLE_H

#include <eflag.h>
#include <cudatalistener.h>

class QuTablePrivate;
class Cumbia;
class CuControlsReaderFactoryI;
class CumbiaPool;
class CuControlsFactoryPool;

class QuTable : public EFlag, public CuDataListener
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:
    QuTable(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    QuTable(QWidget *w, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~QuTable();

    QString source() const;

    int maximumLength() const;

public slots:
    void setSource(const QString& s);

    void unsetSource();

signals:
    void newData(const CuData&da);

protected:

    void configure(const CuData &da);
private:

    void m_init();

    QuTablePrivate *d;

    // CuDataListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUTABLE_H
