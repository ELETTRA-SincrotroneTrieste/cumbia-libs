#ifndef TDIALREAD_H
#define TDIALREAD_H

#include <QDial>


#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>

class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class CuContext;
class CuLinkStats;

class TDialRead : public QDial, public CuDataListener, public CuContextI
{
    Q_OBJECT
public:
    TDialRead(QWidget *w, Cumbia *cumbia, const CuControlsReaderFactoryI &r_fac);

    // CuContextI interface
    virtual CuContext *getContext() const;
    // CuDataListener interface
    virtual void onUpdate(const CuData &data);

    QString source() const;

public slots:
    void setSource(const QString& src);

signals:
    void description(const QString& desc);

private:
    CuContext *context;

};

#endif // TDIALREAD_H

