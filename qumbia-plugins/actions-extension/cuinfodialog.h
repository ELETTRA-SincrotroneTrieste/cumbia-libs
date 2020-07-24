#ifndef CUINFODIALOG_H
#define CUINFODIALOG_H

#include <QDialog>
#include <QLabel>

class CuContextI;
class CuData;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class QFrame;
class CuContext;

class CuInfoDialogPrivate;

/** @private */
class HealthWidget : public QLabel
{
    Q_OBJECT
public:
    HealthWidget(QWidget *parent);

    void paintEvent(QPaintEvent *e);

    void setData(int errcnt, int opcnt);
};

/** \brief a QDialog used by cumbia-qtcontrols to display statistics and link
 *         information about a source or target.
 *
 * This dialog window is shown when a cumbia-qtcontrols widget is clicked with the
 * right button and the *Link stats* action is triggered.
 *
 * The dialog shows:
 * \li the operation count (the number of times a read [write] has been performed)
 * \li the error count
 * \li the last error message
 * \li a green/red bar representing the health of the link (10 read errors over 100
 *     total reads will produce a health of 90%)
 * \li the last operation date and time
 * \li the value
 * \li the write value (if can be applied to the underlying engine)
 * \li the name of the *activity* (e.g. can be polling/event for Tango)
 * \li the data format (scalar, vector)
 * \li the read *mode* (e.g. for Tango can be "EVENT" or "POLLED")
 * \li the last message from the link
 * \li a *live reader* section where a label reads the same value as the monitored
 *     source, but independently.
 * \li a plot showing the trend of the value (read *live* from the info dialog itself)
 *
 * cumbia-qtcontrols widgets that want to display this dialog when the user right clicks
 * on them, have to reimplement the QWidget::contextMenuEvent method and instantiate a
 * CuContextMenu to delegate the initialization of CuInfoDialog.
 * See QuLabel::contextMenuEvent for an example.
 *
 */
class CuInfoDialog : public QDialog
{
    Q_OBJECT
public:
    CuInfoDialog(QWidget *parent);

    virtual ~CuInfoDialog();

    QStringList extractSources(const QString& expression, QString& formula);

    QString extractSource(const QString& expression, QString &formula);

protected:

public slots:
    void liveReadCbToggled(bool start);
    void showAppDetails(bool show);
    void exec(const CuData& in, const CuContext *ctx);

private slots:
    void onMonitorUpdate(const CuData& da);
    void newLiveData(const CuData &d);

private:
    CuContextI* m_ctxwi;
    QWidget *m_senderw;

    void m_makeMonitor(QFrame *monitorF);

    void m_makeLive(QFrame *liveF,  Cumbia* cumbia, const CuControlsReaderFactoryI &r_fac);
    void m_makeLive(QFrame *liveF,  CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);

    CuInfoDialogPrivate* d;
    QString m_makeHtml(const CuData &da, const QString &heading);
    int m_populateAppDetails(QWidget *container);
    void m_resizeToMinimumSizeHint();
    QMap<QString, QString> m_appPropMap() const;

    int mAppDetailsLayoutRow;
};

#endif // CUINFODIALOG_H
