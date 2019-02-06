#ifndef BOTMONITOR_H
#define BOTMONITOR_H

#include <QObject>
#include <botreader.h>

class CuData;
class QString;
class BotMonitorPrivate;
class CumbiaPool;
class CuControlsFactoryPool;


class BotMonitor : public QObject
{
    Q_OBJECT
public:
    explicit BotMonitor(QObject *parent, CumbiaPool *cu_pool, const CuControlsFactoryPool &fpool);

    bool error() const;

    QString message() const;

    BotReader*  findReader(int chat_id, const QString& src) const;

    BotReader*  findReaderByUid(int user_id, const QString& src) const;

signals:
    void alarm(int chat_id, const CuData& data);

    void stopped(int chat_id, const QString& src, const QString& message);

    void started(int user_id, int chat_id, const QString& src, const QString& formula);

    void startError(int chat_id, const QString& src, const QString& message);

    void newData(int chat_id, const CuData& data);

    void onFormulaChanged(int user_id, int chat_id, const QString& src, const QString& old, const QString& new_f);


public slots:

    bool stop(int chat_id, const QString &src);

    bool startRequest(int user_id, int chat_id, const QString& src,
                      const QString& formula, BotReader::Priority priority,
                      const QString& host = QString());

    void readerStartSuccess(int user_id, int chat_id, const QString& src, const QString& formula);

private:
    BotMonitorPrivate *d;

private slots:
    void m_onNewData(int, const CuData&);

    void m_onFormulaChanged(int chat_id, const QString& src, const QString& old, const QString& new_f);
};

#endif // BOTMONITOR_H
