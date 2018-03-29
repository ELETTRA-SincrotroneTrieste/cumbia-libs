#ifndef TDIALREAD_H
#define TDIALREAD_H

#include <QDial>
#include <com_proxy_reader.h>
#include <qtango_wcommon.h>

class TDialRead : public QDial, public QTangoComProxyReader, public QTangoWidgetCommon
{
    Q_OBJECT
public:
    explicit TDialRead(QWidget *parent = nullptr);

signals:
    void description(const QString& desc);


    // QTangoComProxyReader interface
protected slots:
    virtual void refresh(const TVariant &);

    void autoConfigure(const TangoConfigurationParameters *cp);
};

#endif // TDIALREAD_H
