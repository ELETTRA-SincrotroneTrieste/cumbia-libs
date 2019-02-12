#ifndef BOTMONITOR_H
#define BOTMONITOR_H

#include <QObject>
#include <botreader.h>
#include <QDateTime>

class CuData;
class QString;
class BotMonitorPrivate;
class CumbiaPool;
class CuControlsFactoryPool;


class BotMonitor : public QObject
{
    Q_OBJECT
public:

    ~BotMonitor();

    explicit BotMonitor(QObject *parent, CumbiaPool *cu_pool, const CuControlsFactoryPool &fpool, int time_to_live);

    bool error() const;

    QString message() const;

    BotReader*  findReader(int chat_id, const QString& src, const QString& host) const;

    BotReader*  findReaderByUid(int user_id, const QString& src, const QString& host) const;

    QList<BotReader *>  readers() const;

signals:

    void stopped(int user_id, int chat_id, const QString& src, const QString& host, const QString& message);

    void started(int user_id, int chat_id, const QString& src, const QString& host, const QString& formula);

    void startError(int chat_id, const QString& src, const QString& message);

    void newData(int chat_id, const CuData& data);

    void onFormulaChanged(int user_id, int chat_id, const QString& src, const QString& host, const QString& old, const QString& new_f);
    void onMonitorTypeChanged(int user_id, int chat_id, const QString& src, const QString& host, const QString& old_t, const QString& new_t);

public slots:

    bool stop(int chat_id, const QString &src);

    bool stopByIdx(int chat_id, int index);

    bool startRequest(int user_id, int chat_id, const QString& src,
                      const QString& formula, BotReader::Priority priority,
                      const QString& host = QString(),
                      const QDateTime& startedOn = QDateTime());

    void readerStartSuccess(int user_id, int chat_id, const QString& src, const QString& formula);

private:
    BotMonitorPrivate *d;

private slots:
    void m_onNewData(int, const CuData&);

    void m_onFormulaChanged(int chat_id, const QString& src, const QString& old, const QString& new_f);

    void m_onPriorityChanged(int chat_id, const QString& src,
                                BotReader::Priority oldpri, BotReader::Priority newpri);

    void m_onLastUpdate(int chat_id, const CuData& dat);

    int m_findIndexForNewReader(int chat_id);
};

#endif // BOTMONITOR_H