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
class QTextBrowser;
class QTreeWidgetItem;

class CuInfoDialogPrivate;

/** @private */
class HealthWidget : public QLabel {
    Q_OBJECT
public:
    HealthWidget(QWidget *parent);
    void paintEvent(QPaintEvent *e);
    void setData(int errcnt, int opcnt);
};

class CuInfoDEventListener {
public:
    virtual void onObjectChanged(QObject *obj, const CuContextI *ctx) = 0;
};

class CuInfoDEventFilterPrivate;

class CuInfoDEventFilter : public QObject {
    Q_OBJECT
public:
    CuInfoDEventFilter(QObject *parent, CuInfoDEventListener *el);
    ~CuInfoDEventFilter();

    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void notify();

private:
    class CuInfoDEventFilterPrivate *d;
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
class CuInfoDialog : public QDialog, public CuInfoDEventListener
{
    Q_OBJECT
public:
    enum ContentType { Data, Conf };

    CuInfoDialog(QWidget *parent);

    virtual ~CuInfoDialog();

    QStringList extractSources(const QString& expression, QString& formula);

    QString extractSource(const QString& expression, QString &formula);

protected:

public slots:
    void showAppDetails(bool show);
    void exec(const CuData& in, const CuContextI *ctxi);

private slots:
    void onMonitorUpdate(const CuData& da);
    void switchEngine(bool checked);

private:
    CuContextI* m_ctxwi;
    QObject *m_owner;

    void m_makeMonitor(QFrame *monitorF);

    CuInfoDialogPrivate* d;
    int m_populateAppDetails(QWidget *container);
    void m_resizeToMinimumSizeHint();

    QMap<QString, QString> m_appPropMap() const;

    int mAppDetailsLayoutRow;

    void m_one_time_read(const QString& src, const CuContext *ctx);

    // CuInfoDEventListener interface
    QTreeWidgetItem *m_readers_root() const;
    QTreeWidgetItem *m_writers_root() const;

    QList<QTreeWidgetItem *> m_reader_items() const;
    QList<QTreeWidgetItem *> m_writer_items() const;

    QTreeWidgetItem *m_find_reader(const QString& src) const;
    QTreeWidgetItem *m_find_writer(const QString &tgt) const;
    QTreeWidgetItem *m_get_reader_key(const QString &src, const QString& key) const;
    QTreeWidgetItem *m_get_writer_key(const QString &tgt, const QString& key) const;
    QTreeWidgetItem *m_find_child(const QTreeWidgetItem* parent, const QString& key) const;

    QTreeWidgetItem *m_add_reader(const QString& src);
    QTreeWidgetItem *m_add_writer(const QString& tgt);


    QTreeWidgetItem *m_update_reader_key(const QString& src, const QString& key, const QString& value, int column);
    QTreeWidgetItem *m_update_writer_key(const QString& tgt, const QString& key, const QString& value, int column);

    int m_readers_count() const;
    int m_writers_count() const;

    void m_update_props(const CuData& da, bool writer);
    void m_update_value(const CuData& da, bool live);
    void m_update_stats(const QString &src);

public:
    void onObjectChanged(QObject *obj, const CuContextI *ctxi);
};

#endif // CUINFODIALOG_H
