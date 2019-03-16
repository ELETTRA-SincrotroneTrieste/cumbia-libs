#ifndef BotReader_H
#define BotReader_H

#include <QObject>
#include <cudatalistener.h>
#include <cucontexti.h>
#include <cudata.h>

#include <QString>
class QContextMenuEvent;

class BotReaderPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;
class CuControlsFactoryPool;
class CuContext;
class CuLinkStats;

/*! \brief BotReader is a reader that uses QObject to display values.
 *
 * Connection is initiated with setSource. When new data arrives, it is displayed and the newData convenience
 * signal is emitted.
 * 
 * getContext returns a pointer to the CuContext used as a delegate for the connection.
 *
*/
class BotReader : public QObject, public CuDataListener, public CuContextI
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource DESIGNABLE true)

public:

    // message delivered silently or not
    enum Priority { High, Low };

    enum RefreshMode { RefreshModeUndefined, Event, Polled };

    BotReader(int user_id,
              int chat_id, QObject *w,
              CumbiaPool *cumbia_pool,
              const CuControlsFactoryPool &fpool,
              int ttl,
              int poll_period,
              const QString& command, // command that originated the reader
              Priority pri = High,
              const QString& host = QString(),
              bool monitor = false);

    virtual ~BotReader();

    void setPropertyEnabled(bool get_props);

    void setPropertiesOnly(bool props_only);

    QStringList sources() const;

    /** \brief returns the source of the reader
     *
     * @return a QString with the name of the source
     */
    QString source() const;

    bool hasSource(const QString& src) const;

    bool sourceMatch(const QString& pattern) const;

    bool sameSourcesAs(const QSet<QString> &srcset) const;

    bool hasNoFormula() const;

    /**
     * @brief command the command that originated the reader
     *
     * @return the text of the command that originated the reader
     */
    QString command() const;

    /**
     * @brief setCommand change the command string
     * @param cmd
     */
    void setCommand(const QString& cmd);

    QString host() const;

    QDateTime startedOn() const;

    int ttl() const;

    void setStartedOn(const QDateTime &dt);

    bool hasStartDateTime() const;

    /** \brief returns a pointer to the CuContext used as a delegate for the connection.
     *
     * @return CuContext
     */
    CuContext *getContext() const;

    int userId() const;

    int chatId() const;

    Priority priority() const;

    void setPriority(Priority pri);

    int index() const;

    void setIndex(int idx);

    int refreshCount() const;

    int notifyCount() const;

    void setPeriod(int period) const;

    int period() const;

    RefreshMode refreshMode() const;

    QString print_format() const;
    double max() const;
    double min() const;
    QString description() const;
    QString label() const;

public slots:

    /** \brief set the source and start reading
     *
     * @param the name of the source
     */
    void setSource(const QString& s);

    /** \brief disconnect the source
     *
     * remove the source and stop reading
     */
    void unsetSource();

    void setFormula(const QString& command);

signals:
    void newData(int chat_id, const CuData&);
    void lastUpdate(int chat_id, const CuData&);
    void startSuccess(int user_id, int chat_it, const QString& src, const QString& command);
    void formulaChanged(int user_id, int chat_id, const QString& src, const QString& oldf, const QString& new_f, const QString& host);
    void priorityChanged(int chat_id, const QString& src, BotReader::Priority oldpri, BotReader::Priority newpri);
    void modeChanged(BotReader::RefreshMode rm);

protected:


private:
    BotReaderPrivate *d;

    void m_init();

    void m_configure(const CuData& d);

    bool m_publishResult(const CuData& da);

    void m_check_or_setStartedNow();

    void m_checkRefreshModeAndNotify(const std::string& refMode);

    // CuDataListener interface
public:
    void onUpdate(const CuData &d);
};

#endif // QUTLABEL_H
