#ifndef CUINFODIALOG_H
#define CUINFODIALOG_H

#include <QDialog>
#include <QLabel>

class CuContextWidgetI;
class CuData;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class QFrame;

class CuInfoDialogPrivate;

class HealthWidget : public QLabel
{
    Q_OBJECT
public:
    HealthWidget(QWidget *parent);

    void paintEvent(QPaintEvent *e);

    void setData(int errcnt, int opcnt);
};

class CuInfoDialog : public QDialog
{
    Q_OBJECT
public:
    CuInfoDialog(QWidget *parent, Cumbia* cumbia, const CuControlsReaderFactoryI *r_fac);

    CuInfoDialog(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    virtual ~CuInfoDialog();

public slots:
    int exec(QWidget *sender, CuContextWidgetI *sender_cwi);

private slots:
    void onMonitorUpdate(const CuData& d);
    void newLiveData(const CuData &d);

private:
    CuContextWidgetI* m_ctxwi;
    QWidget *m_senderw;

    void m_makeMonitor(QFrame *monitorF);

    void m_makeLive(QFrame *liveF,  Cumbia* cumbia, const CuControlsReaderFactoryI &r_fac);
    void m_makeLive(QFrame *liveF,  CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    CuInfoDialogPrivate* d;
};

#endif // CUINFODIALOG_H
