#ifndef TDIALWRITE_H
#define TDIALWRITE_H

#include <QDial>
// #include <com_proxy_writer.h>
// no cumbia include replacement found for com_proxy_writer.h
// #include <qtango_wcommon.h>
// no cumbia include replacement found for qtango_wcommon.h

class TDialWrite : public QDial, public QTangoComProxyWriter, public QTangoWidgetCommon
{
    Q_OBJECT
public:
    explicit TDialWrite(QWidget *parent = nullptr);

signals:
    void description(const QString& desc);

protected slots:
    void autoConfigure(const TangoConfigurationParameters *cp);

    void write(int val);
};

#endif // TDIALWRITE_H

