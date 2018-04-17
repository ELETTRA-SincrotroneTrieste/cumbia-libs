#ifndef TDIALWRITE_H
#define TDIALWRITE_H

#include <QDial>
#include <cudatalistener.h>
#include <cucontexti.h>

class Cumbia;
class CumbiaPool;
class CuControlsFactoryPool;
class CuControlsWriterFactoryI;

class TDialWrite : public QDial, public CuDataListener, public CuContextI
{
    Q_OBJECT
public:
    explicit TDialWrite(QWidget *parent, Cumbia *cumbia, const CuControlsWriterFactoryI &w_fac);

    QString target() const;
    // CuContextI interface
    virtual CuContext *getContext() const;
    // CuDataListener interface
    virtual void onUpdate(const CuData &data);

signals:
    void description(const QString& desc);

public slots:
    void setTarget(const QString& target);

protected slots:
    void write(int val);

private:
    CuContext *context;
};

#endif // TDIALWRITE_H

